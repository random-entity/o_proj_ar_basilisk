* Firmware = DigiMesh

**TODO** Check CSMA/CD-related parameters

| Parameter                  | Argument (HEX)          | Parameter description                                                            | Argument description            |
| -------------------------- | ----------------------- | -------------------------------------------------------------------------------- | ------------------------------- |
| **Networking**             |                         | Parameters which affect the DigiMesh network                                     |                                 |
| `CH`                       | C (uniformity req'd)    | Operating channel                                                                |                                 |
| `ID`                       | AFAE (uniformity req'd) | Network ID                                                                       |                                 |
| `CE`                       | 2                       | Device Role                                                                      | Non-Routing Module              |
| **Addressing**             |                         | Source and destination addressing settings                                       |                                 |
| `DH`                       | 0                       | Destination Address High                                                         | Broadcast Address               |
| `DL`                       | FFFF                    | Destination Address Low                                                          | Broadcast Address               |
| `RR`                       | 0                       | Unicast Retries: Max # MAC RF packet delivery attempts of unicasts               | Unicast Tx 1 time only          |
| `MT`                       | 0                       | Broadcast Multi-Transmits: # additional broadcast retransmissions                | Broadcast Tx 1 time only        |
| `TO`                       | 40                      | Transmit option (Delivery method)                                                | Point-to-multipoint             |
| **DigiMesh Configuration** |                         | Parameters which affect outgoing transmissions in a DigiMesh network             |                                 |
| `NH`                       | 1 (uniformity req'd)    | Network Hops: Max # hops across the network                                      | Max 1 hop (0 not allowed)       |
| `BH`                       | 1                       | Broadcast Hops: Max transmission hops for broadcast                              | Max 1 hop (0 becomes max hops)  |
| `MR`                       | 0                       | Mesh Unicast Retries: Resent up to MR times if no acknowledgements are received. | No acknowledgements, no resends |
| `NN`                       | 1                       | Network Delay Slots: Max # delay slots before rebroadcasting                     | (0 not allowed)                 |
| **RF Interfacing**         |                         | RF interface options for 2.4 GHz DigiMesh traffic                                |                                 |
| `PL`                       | 2                       | TX Power Level                                                                   | Medium                          |
| **API configuration**      |                         | API mode configuration                                                           |                                 |
| `AP`                       | 2                       | Radio operating mode                                                             | API Mode with escapes           |
| **UART Interface**         |                         |                                                                                  |                                 |
| `BD`                       | 7                       | UART Baud Rate                                                                   | 115200 baud                     |
| **Diagnostics**            |                         | Information about the XBee hardware and firmware                                 |                                 |
| `VR`                       | 3012                    | Firmware Version                                                                 |                                 |

The rest are assumed to be set to their default values.
