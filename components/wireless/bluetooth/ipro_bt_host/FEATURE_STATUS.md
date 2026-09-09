# Host completion tracking

Updated 2026-09-08. Host source/config availability, a linked build, and an
on-air runtime result are different acceptance levels. Do not repeat accepted
tests merely to populate a later report. Consult controller `HANDOFF.md` for
older IPRO7 LE Audio results before scheduling an ISO test.

Owner now explicitly requires all applicable BLE5.3/5.4 Host AND Controller
functionality implemented and tested. The authoritative expanded backlog is
[BLE53_54_COMPLETION.md](../../../../docs/ble/BLE53_54_COMPLETION.md).
Older basic PASS gates below remain valid only for their documented scope.

## Accepted, skip in the current completion queue

- IPRO6S/Android: SSP/SMP and retained bonding, SDP, RFCOMM/SPP exact-content
  sessions, A2DP Sink/SBC decode, AVRCP CT/TG connection, HFP HF and SCO/HCI.
- IPRO6S/IPRO7: GAP/GATT, MTU 247, DLE 251/251, channel map update, 125 ms to
  2 s connection-interval gates, without PDS.
- Ten-profile source/build matrix and compiled import inventory (98 source
  imports, 106 required headers). These are not peer-device qualifications.
- Newly accepted: AVRCP GetPlayStatus, GetElementAttributes, PLAY and PAUSE
  press/release to native Android, including exact fixture values and Android
  media-key effects; playback-state INTERIM/CHANGED notifications and renewal
  across consecutive remote changes. See [runtime report](../../../../docs/ble/reports/20260905-avrcp/REPORT.md).

## Remaining work queue

Owner priority update: complete **non-Audio** LE (including 5.3/5.4) and
BR/EDR Host first. Audio items below are deferred, not prerequisites for this
queue. Do not repeat the accepted Audio gates.

2026-09-05 non-Audio increment: Connection Subrating opt-in config and dual-board
Host/HCI/LL operation accepted for central-requested factors 2, 4, then 1 at a
125 ms base interval. This is a basic functional gate, not long-duration stress
or all-parameter qualification. See `docs/ble/reports/20260905-subrating/REPORT.md`.

2026-09-06 BLE 5.4 increment: the optional LE GATT Security Levels service,
pre-enable configuration API and archive link retention are implemented. An
unsecured IPRO6S server -> IPRO7AI client Read By Type gate returned the exact
two-requirement value 50/50 with no disconnect. This accepts the basic
characteristic only; permission-policy agreement and security-upgrade cases
remain open. See `docs/ble/reports/20260906-gatt-security-levels/REPORT.md`.

2026-09-07 BLE 5.3 increment: peripheral channel classification is accepted in
the IPRO6S peripheral -> IPRO7AI central direction.  The actual LL reporting
path removed and restored selected channels in 6/6 repeated and 10/10
byte/bit-boundary transitions while the ACL remained connected.  This is not
the older Host channel-map command test.  Negative/reverse/multi-link cases
remain open.  See
`docs/ble/reports/20260907-channel-classification/REPORT.md`.

2026-09-07 BLE 5.3 increment: periodic advertising ADI is accepted in both
directions. Ten live payload changes per direction were
reported exactly once each with duplicate filtering (`same=0`, `bad=0`), the
ADI-disabled control reported repeated data, and sync loss/reacquisition
passed. The missing Controller data-change/DID update was fixed in both
codebases. IPRO6S reverse used a bare SID5 advertiser; combining the periodic
payload with an extended marker still hits the documented third-buffer/
resource limit. See
`docs/ble/reports/20260907-periodic-adi/REPORT.md`.

2026-09-07 BLE 5.4 increment: an actual authenticated-LESC read permission now
matches the server's Mode 1 Level 4 declaration. A fresh IPRO6S/IPRO7AI link
rejected the protected read at level 1, completed numeric-comparison Secure
Connections at level 4, then returned exact `IPRO-SC4` data 20/20. Direct SDK
configuration had omitted upstream's default MITM enforcement, and the IPRO6S
fixture initially exposed confirmation without display (therefore No I/O);
both integration gaps are fixed without weakening the attribute. See
`docs/ble/reports/20260907-gatt-security-policy/REPORT.md`.

Next non-Audio gates, in order:

2026-09-08 PAST increment: IPRO7AI local periodic advertising set transferred
over an existing ACL to IPRO6S. Three sync acquisitions and 300 bytewise-correct
34-byte reports passed; invalid parameters, unsubscribe/ignore, delete/retransfer
and intentional advertiser-stop sync loss were exercised. PDS off.
Reverse local-set roles now pass three acquisitions/306 exact reports, including
first-attempt advertiser create/recreate and remote-stop timeout. Unified Host
pending-key update ordering and retired Controller-set reclamation were repaired;
see `docs/ble/reports/20260908-past-reverse/REPORT.md`. Received-sync forwarding
and security/mode/resource cases remain open. Prior forward report:
`docs/ble/reports/20260908-past/REPORT.md`. Do not rerun this basic direction.

1. EATT / LE CoC: EATT three-channel establishment, encryption prerequisite,
   and enhanced-only one-byte GATT read now accepted (2026-09-05); skip this
   basic gate. CoC bidirectional4/48/249/250/251/512/1017/1024-byte pattern,
   both-direction credit withholding/resume and CoC rebuild now accepted;
   see `docs/ble/reports/20260905-coc/REPORT.md`. Encrypted bidirectional data
   plus four-buffer queue saturation/drain/reuse are now observed PASS;
   the early allocation errno is not logged, so exact-error API qualification
   remains open (`SECURE_POOL.md` in the same directory). Still validate larger
   bursts, concurrent channels and security-required service admission. See
   `docs/ble/reports/20260905-eatt/REPORT.md`.
2. Extended/periodic advertising, PAST, PAwR; distinguish controller capability
   from Host implementation. Audit BLE 5.3 channel classification and BLE 5.4
   EAD integration separately. EAD basic library + dual-board air gate is now
   PASS (2026-09-05):30 selftest checks per chip, exact authenticated payload
   and MIC corruption rejection in both directions. See
   `docs/ble/reports/20260905-ead/REPORT.md`. Key distribution/rotation/replay
   policy and PAwR transport remain unqualified.
3. Classic GOEP/OBEX operations and non-Audio profile inventory. HID, PAN,
   PBAP, MAP and OPP must not be marked supported merely because SDP/RFCOMM
   works; their profiles/config/peer tests still require implementation audit.
4. Package a non-ISO BLE 5.3/5.4 controller variant. Current Subrating test
   explicitly enables source build using `bt54_iso2` and 32 KiB EM; Host Audio
   and ISO are disabled, but the controller archive still contains ISO support.

| Function | Existing surface | Next unaccepted gate / required peer |
| --- | --- | --- |
| AVRCP other notifications / browsing | Host sources and config available | Track/volume events, addressed player and browsing responses; playback-state notification already accepted |
| LE Audio VCP/MICP/CAP/CSIP/upper services | Representative builds accepted | Select one service pair and verify discovery, operation and callbacks; reuse earlier CIS baseline evidence |
| EATT and LE L2CAP CoC | Host config and sources present | Peer negotiation, SDU content, flow-control/error behavior |
| Extended/periodic advertising, PAST, PAwR | Host config/build surface present | Check each board's controller feature bits, then new over-air roles only |
| A2DP Source | Build accepted | Need BR/EDR A2DP Sink peer; IPRO7 is LE-only and a stock phone normally acts as Source |
| HFP AG | Build accepted | Need HFP HF peer; current Android phone normally acts as AG |
| GOEP/BIP/OTS and cover art | Build surface present | Matching peer service and object-content/error checks |
| Multipoint / concurrent LE+BR | Not runtime-qualified | Controller capacity/policy plus matching peers; do not infer from single-link gates |
| Upstream release hygiene | Compiled imports present | Package intentional imported-source differences as explicit patches; do not refresh hashes to hide drift |

PDS and IPRO6S cold-reset EM investigations are outside this Host completion
phase by owner direction. IPRO6S's reported lack of LE 2M is a controller
capability boundary, not a Host source omission. An exhaustive Bluetooth/SIG
qualification claim remains out of scope of the existing runtime evidence.
