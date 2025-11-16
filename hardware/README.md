# SUSI Hardware Requirements

This document outlines the hardware requirements for the various SUSI (Serial User Standard Interface) connectors and cables, as defined in the RCN-600 specification.

## Connector Systems

There are three primary SUSI connector systems, each with different mechanical and electrical characteristics.

| Designation | Connector Type | Max Load | Primary Use Case |
| :--- | :--- | :--- | :--- |
| **classicSUSI** | JST SM04B-SRSS-TB | 1000 mA | H0 Scale |
| **microSUSI** | JST SM04B-XSRS-ETB | 200 mA | N Scale |
| **powerSUSI** | Würth WR-WTB 5-pin (2.0mm pitch) | 2000 mA | Large gauges, high-power modules |

---

### 1. classicSUSI Interface

This is the original SUSI interface, widely used since 2003.

*   **Host Connector (Recommended):**
    *   Horizontal: `JST SM04B-SRSS-TB`
    *   Vertical: `JST BM04B-SRSS-TB`
*   **Cable Connector:** `JST 04SR3S` or `JST SRH-04V-S-B`
*   **Pinout (4-pin):**
    1.  **Ground** (Decoder −)
    2.  **Data**
    3.  **Clock**
    4.  **Plus** (Decoder +)
*   **Max Load:** 1000 mA

---

### 2. microSUSI Interface

A smaller form factor designed for space-constrained applications like N scale.

*   **Host Connector (Recommended):** `JST SM04B-XSRS-ETB`
*   **Cable Connector:** `JST 04XSR-36S`
*   **Pinout (4-pin):**
    1.  **Ground** (Decoder −)
    2.  **Data**
    3.  **Clock**
    4.  **Plus** (Decoder +)
*   **Max Load:** 200 mA
*   **Voltage Levels:** Must support both 5V and 3.3V logic levels.

---

### 3. powerSUSI Interface

Introduced for larger scales and modules with higher power demands, such as smoke generators.

*   **Host Connector (Recommended):** Würth Elektronik WR-WTB series (2.0 mm pitch)
    *   Examples: `620 005 116 22` (THT Vertical), `620 105 131 822` (SMT Horizontal)
*   **Pinout (5-pin):**
    1.  **Trigger** (Open-collector signal for synchronization)
    2.  **Ground** (Decoder −)
    3.  **Data**
    4.  **Clock**
    5.  **Plus** (Decoder +)
*   **Max Load:** 2000 mA

---

## Common Properties and Cable Colors

*   **Cable Length:** It is recommended to keep cables between the Host and SUSI module under 20 cm to ensure signal integrity.
*   **Parallel Connection:** Up to 3 SUSI modules can be connected in parallel to a single Host. The total current draw must not exceed the Host's maximum load capacity.
*   **Reverse Polarity Protection:** SUSI modules should include a series diode on the `Plus` connection for protection.

### Recommended Cable Colors (4-pin interfaces)

| Pin | Function | Color |
| :-- | :--- | :--- |
| 1 | Ground | Black |
| 2 | Data | Gray |
| 3 | Clock | Blue |
| 4 | Plus | Red |

## Logic Levels

*   SUSI interfaces can operate at Vcc = 5V or Vcc = 3.3V.
*   All SUSI modules **must tolerate 5V levels**.
*   `microSUSI` modules must also accept **3.3V levels**.
*   Hosts and modules compatible with 3.3V can be marked with "SUSI3".
