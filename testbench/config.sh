#!/bin/bash
# Gemeinsame Einstellungen für run_release.sh und make_report.sh.
# Eigene Werte NICHT hier ändern, sondern in testbench/config.local.sh (wird nicht eingecheckt)
# oder per Umgebungsvariable setzen, z.B.: JOBS=8 ./run_release.sh

TESTBENCH_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$TESTBENCH_DIR/.." && pwd)"

# Testbench-Verzeichnis des VICE-Repos (dort liegen testbench.sh, die Testlisten und die Hooks)
VICE_TESTBENCH="${VICE_TESTBENCH:-$HOME/Programmierung/Aktuelle_Projekte/_not_from_me/vice-emu-code/testprogs/testbench}"

# Zu testende Emu64 Binary (Daten liegen unter ../share daneben, wie in build/install_linux)
EMU64_BIN="${EMU64_BIN:-$REPO_DIR/build/install_linux/bin/emu64}"

# Arbeitsverzeichnis (Kopie der VICE testprogs, Worker-Verzeichnisse, Logs). Liegt unter build/ und ist damit git-ignoriert.
WORK_DIR="${WORK_DIR:-$REPO_DIR/build/testbench_work}"

# Ergebnisse (werden eingecheckt)
RESULTS_DIR="${RESULTS_DIR:-$TESTBENCH_DIR/results}"

# Anzahl paralleler Worker. Jeder Worker startet nacheinander eigene Emu64 Prozesse.
if [ -z "${JOBS:-}" ]; then
    JOBS=$(( $(nproc) / 2 ))
    [ "$JOBS" -gt 12 ] && JOBS=12
    [ "$JOBS" -lt 1 ] && JOBS=1
fi

# Zusätzliche Parameter für testbench.sh. --ciaold entspricht "make testemu64" in der VICE Testbench
# und den bisherigen Ergebnissen (emu64-2019... bis emu64-2025...).
TESTBENCH_FLAGS="${TESTBENCH_FLAGS:---ciaold}"

# Maximale Laufzeit eines einzelnen Emu64 Prozesses in Sekunden (Schutz gegen hängende Prozesse)
EMU64_TIMEOUT="${EMU64_TIMEOUT:-3600}"

[ -f "$TESTBENCH_DIR/config.local.sh" ] && source "$TESTBENCH_DIR/config.local.sh"
