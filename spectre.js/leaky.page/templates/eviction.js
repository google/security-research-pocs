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

const kEndMarker = 0xffffffff;
const kWasmPageSize = 64*1024;
class EvictionList {
  constructor(initialSize, offset) {
    const memorySize = initialSize*PAGE_SZ;
    this.memory = new DataView(new WebAssembly.Memory({initial: Math.ceil(memorySize/kWasmPageSize)}).buffer);
    this.head = offset;
    for (let i = 0; i < initialSize-1; i++) {
      this.memory.setUint32(i*PAGE_SZ+offset, (i+1)*PAGE_SZ+offset, true);
    }
    this.tail = (initialSize-1)*PAGE_SZ+offset;
    this.memory.setUint32(this.tail, kEndMarker, true);
    this.length = initialSize;
  }

  traverse() {
    let e = this.head;
    while (e != kEndMarker) {
      e = this.memory.getUint32(e, true);
    }
    return e;
  }
}
