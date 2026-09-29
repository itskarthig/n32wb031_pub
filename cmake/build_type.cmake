# n32wb03x_apply_build_type(<target>)
#
# Opt-in per-target Release/Debug compile-option switch for the Projects/
# OTA tree (INSTALLER/APP pairs under Projects/N32WB03x_EVAL/Applications/).
# Scoped to the target it's called on via target_compile_options -- has no
# effect on any target that doesn't call it, so the existing top-level apps
# (app_beacon, app_installer, app_ble_*, app_ram_check), whose flags come
# unconditionally from cmake/toolchain-gcc.cmake, are completely unaffected.
#
# Debug trades size for full debuggability (-O0 -g3, no inlining/optimization
# to fight through). Release is deliberately a no-op: since this is called
# AFTER the target's own target_compile_options() in each leaf CMakeLists.txt,
# and the compiler honors the LAST -O flag on the command line, adding -O2
# here would silently override a target's own choice (e.g. INSTALLER uses
# -Os for its tight flash budget) instead of leaving it alone as intended.
function(n32wb03x_apply_build_type target)
    target_compile_options(${target} PRIVATE
        $<$<CONFIG:Debug>:-O0;-g3>
    )
endfunction()
