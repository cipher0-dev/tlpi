#!/bin/bash

# This program demostrates that shell scripts on Linux do not have their
# effective user/group set from set-u/gid bits. To demonstrate this you will
# have to set these bits along with owner/group on the script.

# Real and effective UIDs
echo "UIDs:"
echo "  real      = $UID"
echo "  effective = $EUID"

# Real and effective GIDs
# `id -g -r` returns the effective GID
# `id -g` returns the effective GID too, so for real GID we need `id -r -g` or use /proc/self/status parsing
# Simpler: we can read real GID from /proc/self/status
RGID=$(awk '/^Gid:/ {print $2}' /proc/self/status)
EGID=$(id -g)

echo "GIDs:"
echo "  real      = $RGID"
echo "  effective = $EGID"

# Supplemental groups
echo "Supplemental groups:"
# `id -G` lists all group IDs including the effective GID
# remove EGID to match C getgroups() behavior
SUPP_GROUPS=($(id -G | tr ' ' '\n' | grep -v "^$EGID$"))
for i in "${!SUPP_GROUPS[@]}"; do
    echo "  [$i] ${SUPP_GROUPS[$i]}"
done
