#!/bin/sh
# Copy third party files under web root.
cp -r ../third_party/aurelia/sample-app/* repo/csp/sd
cp -r ../third_party/vue/sample-app/* repo/csp/sd
cp ../third_party/dompurify/purify.js repo/sanitizers/dompurify
cp -r ../third_party/polymer/polymer-start .
# Create a symlink for modsecurity pocs.
ln -sfn ../../../repo repo/waf/modsecurity/repo
# Prepare closure-library poc
git clone https://github.com/google/closure-library repo/sanitizers/closure/closure-library
git -C repo/sanitizers/closure/closure-library checkout 2afbc5e793acc1c0582da9f2325fa6edee8dc85a
