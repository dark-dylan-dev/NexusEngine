#!/usr/bin/env bash
set -euo pipefail

# Required env vars:
#   GH_PAT       - fine-grained PAT scoped to this repo, "Administration: read & write"
#   GH_OWNER     - e.g. "dylan"
#   GH_REPO      - e.g. "nexus"
#
# Optional:
#   RUNNER_NAME  - defaults to hostname
#   RUNNER_LABELS - defaults to "docker-local,linux-x64"

: "${GH_PAT:?Set GH_PAT to a fine-grained PAT with Administration: read/write on the repo}"
: "${GH_OWNER:?Set GH_OWNER (e.g. your github username or org)}"
: "${GH_REPO:?Set GH_REPO (repo name, no owner prefix)}"

RUNNER_NAME="${RUNNER_NAME:-$(hostname)}"
RUNNER_LABELS="${RUNNER_LABELS:-docker-local,linux-x64}"

API="https://api.github.com/repos/${GH_OWNER}/${GH_REPO}"

echo "==> Requesting runner registration token from GitHub..."
REG_TOKEN=$(curl -sX POST \
  -H "Authorization: Bearer ${GH_PAT}" \
  -H "Accept: application/vnd.github+json" \
  "${API}/actions/runners/registration-token" | jq -r .token)

if [[ -z "$REG_TOKEN" || "$REG_TOKEN" == "null" ]]; then
  echo "Failed to get registration token. Check GH_PAT permissions and GH_OWNER/GH_REPO." >&2
  exit 1
fi

echo "==> Configuring runner '${RUNNER_NAME}' with labels [${RUNNER_LABELS}]..."
./config.sh \
  --url "https://github.com/${GH_OWNER}/${GH_REPO}" \
  --token "${REG_TOKEN}" \
  --name "${RUNNER_NAME}" \
  --labels "${RUNNER_LABELS}" \
  --unattended \
  --replace

cleanup() {
  echo "==> Deregistering runner..."
  REMOVE_TOKEN=$(curl -sX POST \
    -H "Authorization: Bearer ${GH_PAT}" \
    -H "Accept: application/vnd.github+json" \
    "${API}/actions/runners/remove-token" | jq -r .token)
  ./config.sh remove --token "${REMOVE_TOKEN}" || true
}
trap cleanup EXIT INT TERM

echo "==> Starting runner..."
./run.sh
