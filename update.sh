#!/bin/bash
cd "$(dirname "$0")"
git add .
git commit -m "Update plugin"
git push origin master
echo "Done!"
