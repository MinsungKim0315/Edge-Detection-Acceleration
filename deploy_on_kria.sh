#!/bin/bash

KRIA_IP5="10.210.1.199"
KIRA_IP1="10.210.1.175"
CURRENT_DIR_NAME=$(basename "$PWD")
WORKING_IP=""
SSH_USER="ubuntu"
SSH_TIMEOUT=10
IP_LIST=("$KRIA_IP5" "$KIRA_IP1")

for ip in "${IP_LIST[@]}"; do
    printf "Trying to connect to %-15s ... " "$ip"
    if ssh -o ConnectTimeout=$SSH_TIMEOUT \
           -o StrictHostKeyChecking=no \
           "$SSH_USER@$ip" "true" 2>/dev/null; then
        echo "Scuccess with IP: $ip"
        WORKING_IP="$ip"
        break
    else
        echo "Failed with IP: $ip"
    fi
done

if [ -z "$WORKING_IP" ]; then
    echo -e "Cannot connect to any of the specified IPs: ${IP_LIST[*]}"
    echo ""
    exit 1
fi

echo -e "\n Deploying to Kria at IP: $WORKING_IP\n"
export KRIA_IP="$WORKING_IP"

# Create a directory with the current username in the kria if not exists
echo "Creating directory /home/ubuntu/$USER/$CURRENT_DIR_NAME on kria at IP '$KRIA_IP'..."
ssh ubuntu@"$KRIA_IP" "mkdir -p /home/ubuntu/$USER/$CURRENT_DIR_NAME"

# Copy kria_dir directory to the kria
echo "Copying kria_dir to kria at IP '$KRIA_IP'..."
scp -r kria_dir/* ubuntu@"$KRIA_IP":/home/ubuntu/"$USER"/"$CURRENT_DIR_NAME"/
scp -r input ubuntu@"$KRIA_IP":/home/ubuntu/"$USER"/"$CURRENT_DIR_NAME"/
