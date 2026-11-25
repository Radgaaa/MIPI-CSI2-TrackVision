# Setup Notes — Toolchain, Debug Probe & First Flash

These are the practical gotchas encountered while getting running the EA iMX RT1176 Developer's Kit (uCOM Carrier Board rev C) via VS Code + MCUXpresso extension + MCU-Link. None of this is obvious from the official docs alone..

## Hardware identification
 - **Board:** Embedded Artists i.MX RT1176 Developer's Kit (uCOM module + uCOM Carrier Board rev C)
 - **SDK:** patched eaimxrt1176_sdk (based on NXP SDK 2.15.000), board id eaimxrt1176 / EA-IMXRT1176 in the MCUXpresso extension.
 - **Debug probe:** standalone MCU-Link (base version).
 - **Camera:** Pcam 5C / OV5640

## SETUP
### Quick reference — connector map used so far
    - J1 -> 12V power in (barrel jack, center positive)
    - J11 -> JTAG/SWD debug (MCU-Link)
    - J20 -> ("C")	MIPI-DSI display / HDMI adapter /40 links/
    - J23 -> MIPI-CSI camera (Pcam 5C / OV5640)
    - J26|J27 -> (USB micro | C) — alternate ISP flashing path
    - J29 -> UART console, Cortex-M7, 115200 8N1
    - J30 -> UART console, Cortex-M4, 115200 8N1
    - SW3|SW4 -> On/Off | Reset


### 1. Power
- just use the 12V adapter in J1
    - Default behavior - powering from the on-board 12V→5V DC/DC (J1  barrel jack), which needs no jumper at all.
- USB-only powering (via J29/J30) requires JP2 in the 2-3 position, which requires soldering a pin header there first (nothing to short otherwise).
    - USB Powering: JP2 (5V supply source selector) has no jumper header soldered from factory — only a bare footprint. 


### 2. Arm GNU Toolchain
     — install manually, don't trust the auto-installer

- The MCUXpresso extension's built-in installer repeatedly fails with:

    - MCUXpresso Installer reported error(s):
        - net\disk_cache\cache_util_win.cc:25] Unable to move the cache: Access is denied. (0x5)
        - No element matched Arm GNU Toolchain==12.2.

This error is persistent and NXP no longer distributes exactly version "12.2" through their installer catalog, so the extension's compatibility check will always fail trying to auto-fetch 12.2. It can be safely ignored once a working 12.2 toolchain is registered manually (see below) — disable the nag via:

    Settings → MCUXpresso: Toolchain Compatibility Recommendations → uncheck.
    Fix: install the toolchain manually.

    1. Download from https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads → Windows hosted cross toolchains → arm-gnu-toolchain-12.2.rel1-mingw-w64-i686-arm-none-eabi.exe
    2. Run the installer (administrator rights). Installs to: C:\Program Files (x86)\Arm GNU Toolchain arm-none-eabi\12.2 rel1
    3.In the MCUXpresso extension, when prompted "toolchain not found / register it now?", click Register. This adds it to the extension's known toolchain list without needing the broken auto-installer.

    Update the project's .vscode/mcuxpresso-tools.json → toolchainPath to point at the folder above if it isn't already.

### 3. MCU-Link firmware — "SWD (DEBUG) disabled"

- Fresh/new MCU-Link units may ship with an **old firmware** that reports:
    - INFO: MCU-Link probe MCU-LINK r0FF CMSIS-DAP V0.078 (...) has SWD (DEBUG) disabled. 
        - Check the SWD jumper on the probe to enable.

        - Despite the message, no physical jumper may actually be present (J5 on the UM11931 base probe defaults to open = enabled). The real fix is a firmware update:

**FIX:** 
 - Locate the MCU-Link Installer's firmware script, e.g. C:\nxp\MCU-LINK_installer_*\scripts\program_CMSIS.cmd.
    - Disconnect MCU-Link from PC and from the target board.
    - Bridge the J3 jumper (Firmware update / ISP jumper) on the MCU-Link PCB itself.
    - Plug MCU-Link into the PC via USB (J3 jumper still installed) — then press Space in the running script.
    - Firmware MCU-LINK_CMSIS-DAP_V2_263.s19 (or newer) gets programmed.
    - Unplug USB, remove the J3 jumper, replug USB → probe boots normally with new firmware and SWD now works.

After this, LinkServer probes / the extension's Debug Probes panel should show the MCU-Link with its serial number under LinkServer (not SEGGER).

### 4. LinkServer: "Exception: No matching devices"

    Debug fails with:

        CRITICAL: Critical error
        ERRMSG: Exception: No matching devices

**Cause:**  the default launch.json device string uses the EA board id as a suffix (MIMXRT1176xxxxx:EA-IMXRT1176), which LinkServer's device database does not recognize (it only knows official NXP board names like MIMXRT1170-EVKB).

**Fix:** — in .vscode/launch.json, under the relevant configuration, add an explicit linkserver device override without the board suffix:

        jsonc
        "gdbServerConfigs": {
        "linkserver": {
            "device": "MIMXRT1176xxxxx"
        },
        "segger": {},
        "pemicro": {}
        },

Also worth noticing:

- *"executable":* { "elf": "${workspaceFolder}/armgcc/debug/<project>.elf" } is populated (an empty string also causes "GDB server quit unexpectedly" with no useful error in Debug Console — check the Terminal tab instead, that's where LinkServer's actual stderr shows up)
- *"probeSerialNumber"* set to the MCU-Link's serial (from the Debug Probes panel) if more than one probe/tool (SEGGER, PEmicro) is installed.

### 5. If the board stops responding -> flash/erase

        Press and hold ISP_ENABLE
        Press and hold RESET (SW3/SW4)
        Release RESET 
        Wait 1-2 seconds
        Release ISP_ENABLE

### 6. Camera (Pcam 5C / OV5640) — connector & example notes

- Connects to J23 (RPi-compatible 15-pos FPC). 
    - Cable orientation: blue side faces as marked on the camera module itself on that end
    - on the J23 end, blue side faces toward the inner part of the PCB.



