#pragma once
#ifndef MMP_EXTRACTOR_ENTRANCES_H
#define MMP_EXTRACTOR_ENTRANCES_H

#include <string>
#include <common613/compat/file_system.h>

void assemble_single(const common613::filesystem::path& base_dir,
                     const std::string& body_name,
                     int index,
                     int eyes,
                     int mouth,
                     const common613::filesystem::path& output_file);

void assemble_scan(const common613::filesystem::path& base_dir,
                   const common613::filesystem::path& output_dir);

#endif //MMP_EXTRACTOR_ENTRANCES_H
