;; Copyright 2021 Google LLC
;;
;; Licensed under the Apache License, Version 2.0 (the "License");
;; you may not use this file except in compliance with the License.
;; You may obtain a copy of the License at
;;
;;     https://www.apache.org/licenses/LICENSE-2.0
;;
;; Unless required by applicable law or agreed to in writing, software
;; distributed under the License is distributed on an "AS IS" BASIS,
;; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; See the License for the specific language governing permissions and
;; limitations under the License.

(module
  (import "env" "mem" (memory 8192 8192))
  (import "env" "plruCallback" (func $plruCallback (param $trash i32) (result i32)))

  (func (export "oscillateTreePLRU")
    (param $reps i32)
    (param $entry0 i32)
    (param $entry1 i32)
    (param $entry2 i32)
    (param $entry3 i32)
    (param $keepAlive i32)
    (param $entry4 i32)
    (param $entry5 i32)
    (param $entry6 i32)
    (param $clearSet0 i32)
    (param $clearSet1 i32)
    (param $clearSet2 i32)
    (param $clearSet3 i32)
    (param $clearSet4 i32)
    (param $clearSet5 i32)
    (param $clearSet6 i32)
    (param $clearSet7 i32)

    i32.const 0

    local.get $clearSet0
    i32.add
    i32.load
    local.get $clearSet1
    i32.add
    i32.load
    local.get $clearSet2
    i32.add
    i32.load
    local.get $clearSet3
    i32.add
    i32.load
    local.get $clearSet4
    i32.add
    i32.load
    local.get $clearSet5
    i32.add
    i32.load
    local.get $clearSet6
    i32.add
    i32.load
    local.get $clearSet7
    i32.add
    i32.load

    local.get $entry0
    i32.add
    i32.load
    local.get $entry1
    i32.add
    i32.load
    local.get $entry2
    i32.add
    i32.load
    local.get $entry3
    i32.add
    i32.load
    local.get $keepAlive
    i32.add
    i32.load
    local.get $entry4
    i32.add
    i32.load
    local.get $entry5
    i32.add
    i32.load
    local.get $entry6
    i32.add
    i32.load

    call $plruCallback

    loop $forloop (param i32) (result i32)
      local.get $entry0
      i32.add
      i32.load
      local.get $entry1
      i32.add
      i32.load
      local.get $entry2
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry3
      i32.add
      i32.load
      local.get $entry4
      i32.add
      i32.load
      local.get $entry5
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry6
      i32.add
      i32.load
      local.get $entry0
      i32.add
      i32.load
      local.get $entry1
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry2
      i32.add
      i32.load
      local.get $entry3
      i32.add
      i32.load
      local.get $entry4
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry5
      i32.add
      i32.load
      local.get $entry6
      i32.add
      i32.load
      local.get $entry0
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry1
      i32.add
      i32.load
      local.get $entry2
      i32.add
      i32.load
      local.get $entry3
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry4
      i32.add
      i32.load
      local.get $entry5
      i32.add
      i32.load
      local.get $entry6
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $reps
      i32.const 3
      i32.sub
      local.set $reps
      local.get $reps
      i32.const 0
      i32.ge_s
      br_if $forloop
    end
    drop
  )

  (func (export "oscillateTreePLRU2")
    (param $reps i32)
    (param $entry0 i32)
    (param $entry1 i32)
    (param $entry2 i32)
    (param $entry3 i32)
    (param $keepAlive i32)
    (param $entry4 i32)
    (param $entry5 i32)
    (param $entry6 i32)
    (param $clearSet0 i32)
    (param $clearSet1 i32)
    (param $clearSet2 i32)
    (param $clearSet3 i32)
    (param $clearSet4 i32)
    (param $clearSet5 i32)
    (param $clearSet6 i32)
    (param $clearSet7 i32)

    i32.const 0

    local.get $clearSet0
    i32.add
    i32.load
    local.get $clearSet1
    i32.add
    i32.load
    local.get $clearSet2
    i32.add
    i32.load
    local.get $clearSet3
    i32.add
    i32.load
    local.get $clearSet4
    i32.add
    i32.load
    local.get $clearSet5
    i32.add
    i32.load
    local.get $clearSet6
    i32.add
    i32.load
    local.get $clearSet7
    i32.add
    i32.load

    local.get $entry0
    i32.add
    i32.load
    local.get $entry1
    i32.add
    i32.load
    local.get $entry2
    i32.add
    i32.load
    local.get $entry3
    i32.add
    i32.load
    local.get $keepAlive
    i32.add
    i32.load
    local.get $entry4
    i32.add
    i32.load
    local.get $entry5
    i32.add
    i32.load
    local.get $entry6
    i32.add
    i32.load

    call $plruCallback

    loop $forloop (param i32) (result i32)
      local.get $entry0
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry1
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry2
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry3
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry4
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry5
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry6
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry0
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry1
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry2
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry3
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry4
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry5
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry6
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry0
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry1
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry2
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry3
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $entry4
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry5
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load
      local.get $entry6
      i32.add
      i32.load
      local.get $keepAlive
      i32.add
      i32.load

      local.get $reps
      i32.const 3
      i32.sub
      local.set $reps
      local.get $reps
      i32.const 0
      i32.ge_s
      br_if $forloop
    end
    drop
  )
)
