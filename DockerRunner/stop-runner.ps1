# stop-runner.ps1
# Stops the runner container. The container's own trap (in entrypoint.sh)
# deregisters it from GitHub automatically as it shuts down.
#
# Usage: .\stop-runner.ps1

Write-Host "==> Stopping runner container..." -ForegroundColor Cyan
docker compose down

Write-Host "==> Done. Check GitHub -> repo -> Settings -> Actions -> Runners to confirm it's gone." -ForegroundColor Green
