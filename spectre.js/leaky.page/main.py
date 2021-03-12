#!/usr/bin/env python3
#
# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# -*- coding: utf-8 -*-

from flask import Flask, render_template, request, Response, abort, send_from_directory, redirect
import os

app = Flask(__name__)

frame_origin = 'http://127.0.0.1:8080'
gae_application_var = os.getenv('GAE_APPLICATION')
if gae_application_var:
    gae_application = gae_application_var.split('~')[1]
    frame_origin = 'https://victim-dot-{}.appspot.com'.format(gae_application)

def get_arg(name):
    val = request.args.get(name, '')
    if val == '':
        raise 'missing required arg ' + name
    return val

@app.route('/favicon.ico')
def favicon():
    return send_from_directory('static', 'favicon.ico')

@app.route('/preview.png')
def previewpng():
    return send_from_directory('static', 'preview.png')

@app.route('/blog')
def blog():
    return redirect('https://security.googleblog.com/2021/03/a-spectre-proof-of-concept-for-spectre.html')

@app.route('/')
def intro():
    return render_template('intro.html')

@app.route('/plru.html')
def plru():
    return render_template('plru.html')

@app.route('/timer.html')
def l1timer():
    macos = "Mac OS X" in request.headers.get('User-Agent')
    return render_template('timer.html', frame_origin=frame_origin, macos=macos)

@app.route('/timer_frame.html')
def l1timer_frame():
    template_args = {
      'l1_reps': get_arg('l1_reps'),
      'test_reps': get_arg('test_reps'),
      'stable_timer': get_arg('stable_timer'),
      'page_size': 0x4000 if request.args.get('m1_cpu', '') == 'true' else 0x1000,
    }
    return render_template('timer_frame.html', **template_args)

@app.route('/memory.html')
def memory():
    macos = "Mac OS X" in request.headers.get('User-Agent')
    return render_template('memory.html', frame_origin=frame_origin, macos=macos)

@app.route('/memory_frame.html')
def memory_frame():
    template_args = {
      'l1_reps': get_arg('l1_reps'),
      'cache_threshold': get_arg('cache_threshold'),
      'measurements': get_arg('measurements'),
      'measurement_reps': get_arg('measurement_reps'),
      'stable_timer': get_arg('stable_timer'),
      'page_size': 0x4000 if request.args.get('m1_cpu', '') == 'true' else 0x1000,
    }
    return render_template('memory_frame.html', **template_args)

@app.route('/spectre.html')
def spectre():
    macos = "Mac OS X" in request.headers.get('User-Agent')
    return render_template('spectre.html', frame_origin=frame_origin, macos=macos)

noopt_code = 'let foo = 0; ' + '; '.join('foo++' for _ in range(12000))

@app.route('/spectre_frame.html')
def spectre_frame():
    return render_template('worker_frame.html', worker_js='spectre_worker.js')

@app.route('/spectre_worker.js')
def spectre_worker():
    template_args = {
      # these values work for Chrome 88
      'typed_array_offset': 0xa4,
      'backing_store_offset': 0xa4,
      'backing_store_ptr_offset': 0x28,

      'noopt_code': noopt_code,
      'l1_reps': get_arg('l1_reps'),
      'cache_threshold': get_arg('cache_threshold'),
      'measurements': get_arg('measurements'),
      'measurement_reps': get_arg('measurement_reps'),
      'stable_timer': get_arg('stable_timer'),
      'gadget_loop_reps': get_arg('gadget_loop_reps'),
      'training_reps': get_arg('training_reps'),
      'eviction_list_size': get_arg('eviction_list_size'),
      'min_leak_reps': get_arg('min_leak_reps'),
      'max_leak_reps': get_arg('max_leak_reps'),
      'page_size': 0x4000 if request.args.get('m1_cpu', '') == 'true' else 0x1000,

      'leak_test_reps': 1000,
      'accepted_false_positive_percent': 20,
      'accepted_false_negative_percent': 20,
      'leak_count': 4096,
      'report_size': 128,
    }
    return Response(render_template('spectre_worker.js', **template_args), mimetype='text/javascript')

@app.route('/static/<path:path>')
def static_dir(path):
    return send_from_directory('static', path)

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=8080, debug=True)
