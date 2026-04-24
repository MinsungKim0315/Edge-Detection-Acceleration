#!/bin/bash

set -euo pipefail

KRIA_IP="10.210.1.175"
SSH_USER="ubuntu"
CURRENT_DIR_NAME=$(basename "$PWD")
REMOTE_DIR="/home/ubuntu/$USER/$CURRENT_DIR_NAME"

echo "Deploying to Kria at IP: $KRIA_IP"
echo "Remote directory: $REMOTE_DIR"

echo "Creating remote directories..."
ssh "${SSH_USER}@${KRIA_IP}" "mkdir -p '${REMOTE_DIR}' '${REMOTE_DIR}/output'"

echo "Copying kria_dir contents..."
scp -r kria_dir/. "${SSH_USER}@${KRIA_IP}:${REMOTE_DIR}/"

echo "Copying input directory..."
scp -r input "${SSH_USER}@${KRIA_IP}:${REMOTE_DIR}/"

echo "Loading bitstream on Kria..."
ssh -t "${SSH_USER}@${KRIA_IP}" "cd '${REMOTE_DIR}' && sudo load_bitstream bitstream.bit"

cat <<EOF

Deployment complete.

Next steps on the Kria:
  ssh ${SSH_USER}@${KRIA_IP}
  cd ${REMOTE_DIR}
  make fpga
  ./edge_detector_fpga input/bird.pgm output/bird_edge.pgm --low 20 --high 60

EOF
