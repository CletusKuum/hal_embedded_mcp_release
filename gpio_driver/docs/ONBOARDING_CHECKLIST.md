# Onboarding Environment Checklist

Use this checklist to ensure your environment is correctly set up for HAL Driver development.

## 1. System Requirements
- [ ] **OS**: Windows 10/11 or Linux (Ubuntu 20.04+ recommended).
- [ ] **Git**: Installed and authenticated.
- [ ] **Terminal**: PowerShell (Windows) or Bash.

## 2. Toolchain Verification
- [ ] **GCC Compiler**:
    - Run `gcc --version`
    - *Expected*: valid output (e.g., MinGW or build-essential).
- [ ] **Python**:
    - Run `python --version` (or `python3`)
    - *Expected*: Python 3.8 or higher.
- [ ] **Unity**:
    - Check Unity Hub for installed Editors.
    - *Expected*: 2022.3.x (LTS) or compatible.

## 3. Network Ports
- [ ] **Port 8080**:
    - Ensure no other service is blocking port 8080.
    - The Simulator defaults to this port.

## 4. Repository Setup
- [ ] **Clone**: Repo cloned to a path without spaces (preferred).
- [ ] **Submodules**: If applicable, ensure submodules are updated (though this repo appears monolithic).

## 5. Quick Test (The "Smoke Test")
- [ ] Start Simulator (`python gpio_simulator.py`).
- [ ] Send manual curl (optional):
    ```bash
    curl -X POST http://localhost:8080/api/gpio/TESTPIN -d "{\"value\": 1}"
    ```
    *Expected*: Simulator logs the request.

## 6. Unity Setup
- [ ] Project opens without "Script Missing" errors.
- [ ] Console in Unity is clear of red errors upon play.
