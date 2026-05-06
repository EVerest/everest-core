# LocalAllowlistTokenValidator

## Description

The **LocalAllowlistTokenValidator** module provides a `token_provider` interface.

It requires a `.txt` file containing a list of tokens (**one token per line**), optionally followed by a list of `evse_ids`.

If the same token is placed multiple times in an allow list, then the first entry will be used.

These `evse_ids` are processed by an Auth module to determine whether a specific `evse_id` is authorized.

Entries in the `.txt` file must follow a specific format, but the parser is designed to be relatively robust against variations in formatting.

---

## Format Rules & Examples

| Status | Example                                | Description                                                                     |
| ------ | -------------------------------------- | ------------------------------------------------------------------------------- |
| 🟩 ✔️  | `04794b22c11c90`                       | No `evse_id` specified → token is **not restricted**                            |
| 🟩 ✔️  | `04794b22C11C90 17`                    | Token is **case-insensitive**; single `evse_id` supported                       |
| 🟩 ✔️  | `04794B22c11C90 1,2,3`                 | Multiple `evse_ids` must be **comma-separated**                                 |
| 🟩 ✔️  | `04794b22c11c90 -1, 2,  3,4,  5`  | **Whitespace is ignored**, as long as token and list are separated by space/tab |
| 🟩 ✔️  | `04794b22c11c90 "10,1,B,3,17,@,2e,21"` | Only **integer values are extracted** → result: `[10, 1, 3, 17, 2, 21]`         |
| 🟩 ✔️  | `04794b22c11c90 2.3, 4.5`              | **Float values are converted to integers**                                      |
| 🟥 ❌   | `04794b22c11c90,1,2,3,4`               | Missing space/tab between token and `evse_ids` → invalid                        |
| 🟥 ❌   | `04794b22c11c90 1 2 3 4 5`             | Without commas, **only the first `evse_id` is used**                            |

---

## Module Configuration Parameters

### `allowlist_file`

* **Type:** `string`
* **Default:** `/etc/everest/allowlist_rfid.txt`
* **Description:** Path and filename of the file containing one RFID token and optional `evse_id(s)` per line.

---

## Important Notes

* Separator between token and `evse_ids`: **space or tab required**
* Separator between `evse_ids`: **comma**
* Token case sensitivity: **not relevant**
* Invalid characters in `evse_id` lists are **ignored**
* Decimal values are automatically **converted to integers**

---

## Example of a Valid File

```txt id="f7k2p1"
04794b22c11c90
0579EG4FC14C08 17
15164222c11744 10,1,3,17,2,21
```
