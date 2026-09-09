# 套件管理中心與動態線上安裝（3.4.0）

## 管理位置

| 內容 | 位置 |
| --- | --- |
| 公開套件清單／版本歷史 | [GitHub packages/index.json](https://github.com/INTELPRO-INC/IPRO-SDK/blob/main/packages/index.json) |
| SDK、toolchain、工具包等 ZIP | 同一倉庫的 GitHub Releases |
| 管理頁 | 本機啟動 `Start-Package-Manager.ps1`，只監聽 `127.0.0.1` |
| 支援動態版本的客戶安裝程式 | [Online Setup 3.4.0](https://github.com/INTELPRO-INC/IPRO-SDK/releases/tag/online-installer-v3.4.0) |

GitHub 保存檔案與正式清單，本機管理頁負責準備、審閱、發布及設定穩定版。管理者使用自己的 GitHub CLI 登入，需具備該倉庫寫入權限；客戶端一律使用公開 HTTPS，不需登入或 Token。不要把管理頁暴露到網際網路。

在 `C:\Work\IRPO\ipro-sdk-extension` 開啟 PowerShell：

```powershell
node scripts/package-manager.mjs serve --open
```

也可執行根目錄的 `Start-Package-Manager.ps1`。終端機會顯示本次的本機網址；按 Ctrl+C 停止。需要 Node.js（此機驗證版本 24.11.1）與已登入的 GitHub CLI。若 GH 不在 `C:\Program Files\GitHub CLI\gh.exe`，用 `IPRO_GH_PATH` 指定完整執行檔路徑。管理工具也以獨立原始碼 ZIP 附在 3.4.0 Release，解壓後可使用相同啟動方式。

## 發布單一套件

1. 在管理頁選擇 SDK、toolchain、build tools、IntelProCube、OpenOCD 或 Extension。
2. 填入**新版本號**、已解壓的套件根目錄、發行說明與已驗證的相容依賴。SDK 來源的 `VERSION.txt` 必須與新版本號一致；Extension 根目錄需有對應版本的 x64／ARM64 VSIX。
3. 按「準備與檢查」。只會在本機 `package-manager-output/candidate-*` 產生 ZIP、雜湊、package.json 與審閱報告，這一步不會上傳。
4. 檢查排除項目及敏感檔名提示，完成實際編譯／功能測試與散布授權審閱。準備程序會排除 `.git`、`.vscode`、`build` 與 Python 快取，但檔名提示不是完整祕密掃描，也不能代替人工審閱。
5. 勾選「已審閱…同意公開發布」。若要讓後續 `latest` 選擇使用這版，再勾選「同時設為穩定版」。
6. 按「公開發布至 GitHub」。管理工具建立單獨的 `pkg-<套件>-v<版本>` Release，核對遠端檔案大小與 SHA-256，正式發布後才更新 `packages/index.json`。

只更新 OpenOCD 不會重新壓縮或上傳 SDK、toolchain 等其他套件。ZIP 與已登錄的版本內容不覆寫；新內容使用新版本號。SDK 套件發布只處理發行 ZIP，**不會自動將來源資料夾的變更 commit 到 SDK main 分支**；SDK Git 原始碼歷史請依原本開發流程提交。

如果 Release 已發布但更新清單遇到權限／並行編輯衝突，舊清單仍有效。保留 candidate，檢查後重試；工具會比對已存在的 Release，不強制覆寫。已完整登錄的相同版本重跑會視為完成，不重寫清單。若草稿上傳不完整，需先檢查草稿與本機產物；工具不會為了重試刪除正式版本。

## latest、指定版本與相容性

每個套件都有自己的 `stable` 指標。這裡的 `latest` 指向維護者選定的穩定版，**不是最大的版本字串，也不是 GitHub 全倉庫的 Latest Release**。

新版 EXE 每次啟動安裝流程會讀取 GitHub 清單，版本選擇頁提供：

- `latest (stable / compatible)`：使用穩定版；完整模式依套件宣告調整依賴為相容版本。
- 指定版本：選擇清單保留的某個版本，不跟隨 stable。

相容依賴採用精確版本陣列。例如 SDK 已驗證支援兩種工具鏈：

```json
{
  "toolchain": ["13.1.1-20230713", "14.0.0"],
  "tools": ["2026.09.09"],
  "extension": ["3.3.0"]
}
```

以上是格式示例，不表示 14.0.0 已發布或驗證。依賴必須先存在於正式清單；尚未驗證的組合不要宣告相容。

完整模式會檢查選取組合。若最新 SDK 仍要求舊工具鏈，預設會保留該相容工具鏈，而不是硬裝最新工具鏈；使用者明確指定不相容版本時會停止。單一套件模式不會擅自下載其他依賴，也**不會偵測或保證與客戶目前環境相容**，需依套件要求自行確認。

只要目錄結構與 catalog schema 仍受 3.4.0 支援，未來發布套件並更新 stable 後，客戶不必重新下載另一支 EXE。新增安裝能力、變更清單格式或修正安裝器本身時，仍需發新版 bootstrapper。

## 回復穩定版

管理頁右側選套件與已發布的舊版本，確認後按「更新穩定版」。這只影響之後選 `latest` 的安裝，不刪除新版、不改動現有客戶電腦。清單變更會留下 Git commit，可追蹤發布與 stable 調整歷史。

## 客戶安裝與單獨更新

**3.3.0 EXE 仍固定版本，客戶需先換成 3.4.0 一次。**

3.4.0 提供 Full、SDK、toolchain、build tools、IntelProCube、OpenOCD、Extension 七種模式。所有模式都安裝至新目錄；依舊保留下載快取與舊版目錄。User settings／PATH 變更預設不勾選：

- SDK：可選只切換 `ipro.sdkRoot`。
- Toolchain：可選只切換 `ipro.toolchainPath`。
- Build tools：可選只切換 CMake／Ninja／Python 路徑。
- IntelProCube：可選只切換 `ipro.flashToolPath`。
- OpenOCD：只安裝檔案，不自動設定 Extension／PATH，需在使用 OpenOCD 的工具或啟動配置中指定新路徑。
- Extension：可選安裝 VSIX，不改 SDK／工具設定。

命令列也能指定版本，例如：

```powershell
# 下列目錄必須尚不存在。SDK 1.2.1 不跟隨 stable。
Start-Process .\IPRO-SDK-Online-Setup-3.4.0.exe -Wait -ArgumentList '/MODE=sdk', '/SDKVERSION=1.2.1', '/DIR="C:\IPRO\versions\sdk-pinned-new"', '/TASKS=""'
```

其他版本參數為 `/TOOLCHAINVERSION=`、`/TOOLSVERSION=`、`/FLASHVERSION=`、`/OPENOCDVERSION=`、`/EXTENSIONVERSION=`，預設皆為 `latest`。

`/OFFLINE=1` 使用快取的清單與 ZIP；無快取清單時使用 EXE 內附的初始清單，不會假稱知道離線期間的最新版。`/CATALOG=<本機JSON>` 可明確指定已核准的離線清單，仍會做格式、下載來源與路徑驗證。網路模式讀取清單失敗會停止，不會靜默改裝舊版本。GitHub 的 CDN 快取可能讓剛更新的清單延遲一小段時間顯示。

## 信任與限制

- 動態清單的信任來源是固定 GitHub 倉庫的 HTTPS 內容與維護者寫入權限；不是一個另行簽章的套件索引。ZIP 逐一核對清單中的 SHA-256；EXE 目前仍未 Authenticode 簽章。
- 清單更新使用檔案 SHA 做並行修改保護；發生衝突不會強制覆蓋他人的更新。清單僅支援六個既定套件族群與目前 Windows 目錄結構，不是任意程式的通用套件管理器。
- 客戶按需執行安裝，不會在背景自動替換現有 SDK／工具鏈。設定與 PATH 後續工作不是全域交易；失敗需查看 log，已完成的工作不會自動撤銷。
- 實測平台為 Windows x64；沒有新增 ARM64 實機或板上燒錄測試。相容清單是維護者的測試宣告，不代替實測。

## 此次驗證

已驗證六種單一套件及完整安裝模式、匿名讀取正式清單、匿名套件下載、13 個 catalog 單元測試、18 個既有相關回歸測試，以及範例編譯。只在本機建立模擬 stable 變更，驗證相同 EXE 可選最新／指定舊版及擋下不相容版本；未把模擬版本發布至 GitHub。管理端已測試準備套件、發布審閱門檻、既有版本／SDK 版號保護，以及重試不改動既有清單。
