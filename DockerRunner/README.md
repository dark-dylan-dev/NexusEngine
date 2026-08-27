# Self-hosted GitHub Actions runner (Docker, Windows)

Makes your `git push` / PRs trigger a real job that runs on your machine,
inside Docker, and shows up in the repo's **Actions** tab exactly like a
GitHub-hosted run would.

## Limitation, stated plainly

This runner is a Linux container (Docker Desktop on Windows runs Linux
containers via WSL2). It can genuinely execute the `ubuntu-latest` +
gcc/clang matrix legs. It **cannot** become real Windows/MSVC — `cl` isn't
installable or runnable inside a Linux container, regardless of the host
OS. The provided `ci.yml` handles the `windows-latest`/`cl` leg as a
mingw-w64 cross-compile (build-only, no test execution) on this same
container, and labels it as such in the job output.

## One-time setup

1. **Docker Desktop** — confirm it's running (you said it's already
   installed, good).

2. **Fine-grained PAT**: GitHub -> avatar -> Settings -> Developer settings ->
   Personal access tokens -> Fine-grained tokens -> Generate new token.
   Scope to this repo only, permission: **Administration: Read and write**.

3. Copy `start-runner.ps1.example` to `start-runner.ps1` and fill in your
   three values (`GH_PAT`, `GH_OWNER`, `GH_REPO`):
   ```powershell
   Copy-Item start-runner.ps1.example start-runner.ps1
   notepad start-runner.ps1
   ```
   `start-runner.ps1` is gitignored -- it holds your real token, never commit it.

4. Replace your repo's `.github\workflows\ci.yml` with the one provided
   earlier (or merge the `runs-on` / matrix changes into your existing one).

## Running the runner

From PowerShell, in this folder:

```powershell
.\start-runner.ps1
```

First run may prompt an execution-policy warning. If PowerShell refuses to
run the script, either right-click the file -> "Run with PowerShell", or run
once per session:
```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

This builds the image, registers the container as a runner against your
repo, and leaves it running in the background. Check GitHub -> repo ->
**Settings -> Actions -> Runners** -- it should show as **Idle**.

Push a commit or open a PR -- the run appears in the **Actions** tab as
normal, executing on your container.

Stop it (also deregisters automatically):
```powershell
.\stop-runner.ps1
```

View live logs any time:
```powershell
docker compose logs -f
```

## Notes

- Runner labels: `docker-local, linux-x64, self-hosted` -- `ci.yml`'s
  `runs-on: [self-hosted, docker-local, linux-x64]` targets exactly this
  container.
- `restart: unless-stopped` in `docker-compose.yml` means Docker Desktop
  restarting (e.g. after your PC reboots) brings the runner back up and it
  re-registers automatically.
- The runner checks out your repo itself per-job over the network -- no
  local repo path or bind mount needed.
