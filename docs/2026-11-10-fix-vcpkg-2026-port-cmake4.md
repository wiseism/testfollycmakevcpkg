# 2026-11-10 vcpkg double-conversion 配置失败修复

## 现象

`CMake on a single platform` workflow 在 `vcpkg install folly gtest` 阶段失败：

```
CMake Error at scripts/cmake/vcpkg_execute_required_process.cmake:112 (message):
    Command failed: /usr/local/bin/ninja -v
    ...
error: building double-conversion:x64-linux failed with: BUILD_FAILED
```

并且日志先出现大量 CMP0174 告警（`INTERPRETER keyword ... empty string`）。

## 根因

workflow 里 vcpkg 用 `git checkout 2023.12.12` 锁死在 2023 年的旧 port，而 2026 年的 runner 带上了 CMake 4.x（移除了 CMake<3.5 兼容、CMP0174 行为变更等），旧 port 与新 CMake 不兼容，folly 的双精依赖 double-conversion 在 configure 阶段即失败。

## 修复

`.github/workflows/cmake-single-platform.yml`：

- vcpkg 改为 `git clone --depth 1 --branch 2026.10.16`（当前最新 tag，全部 port 已适配 CMake 4）；
- 附带收益：浅克隆 + 固定 tag，clone 时间从数分钟降到数十秒。

## 参考

- vcpkg release tag: https://github.com/microsoft/vcpkg/releases/tag/2026.10.16
