// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <memory>

#include "registers.hpp"

namespace modbus {
namespace registers {
namespace registry {

/**
 * @brief A subregistry, to be extended by the user to create a registry
 * container for a specific set of registers. A server should use the
 * \c ComplexRegisterRegistry to write and read data from the registers.
 *
 */
class ComplexRegisterSubregistry {
protected:
    std::vector<std::unique_ptr<complex_registers::ComplexRegisterUntyped>> registers;

public:
    ComplexRegisterSubregistry();

    /**
     * @brief Add a register to the subregistry, the register must be allocated on
     * the heap
     *
     * @warning The subregistry will take ownership of the register and delete it
     * when the subregistry is deleted thus the register must be allocated on the
     * heap
     *
     * @param reg The register to be added
     */
    void add(complex_registers::ComplexRegisterUntyped* reg);

    /**
     * @brief Verify that no registers overlap in the address space for this
     * specific subregistry
     *
     * @throw std::runtime_error if registers overlap
     */
    void verify_internal_overlap();

    /**
     * @brief Get the registers in the subregistry
     *
     * @return std::vector<ComplexRegisterUntyped*> The registers in the
     * subregistry. The pointers are owned by the subregistry and live as long as
     * the subregistry lives
     */
    std::vector<complex_registers::ComplexRegisterUntyped*> get_registers();
};

/**
 * @brief A per-server global registry for complex registers. The registry
 * contains \c ComplexRegisterSubregistry shared pointers that contain the
 * actual registers.
 *
 * The Registry's main purpose is to map incoming writes and reads to the
 * correct registers
 *
 */
class ComplexRegisterRegistry {
protected:
    std::vector<std::shared_ptr<ComplexRegisterSubregistry>> registries;

    /**
     * @brief Get a vector of all currently available registers in all
     * subregistries combined
     *
     * @note This is a internal helper function
     *
     * @return std::vector<complex_registers::ComplexRegisterUntyped*> all
     * registers
     */
    std::vector<complex_registers::ComplexRegisterUntyped*> get_all_registers();

public:
    /**
     * @brief Verify that no registers overlap in the address space
     *
     * @throw std::runtime_error if registers overlap
     */
    void verify_overlap();

    /**
     * @brief Write data to the registers, maps received data to the correct
     * registers
     *
     * @param address The start address of the data
     * @param data The data to be written to the registers (if derived from uint16
     * registers must be in big-endian order). Must also be a multiple of 2 bytes
     *
     * @throw std::runtime_error if data size is not a multiple of 2 bytes
     */
    void on_write(std::uint16_t address, const std::vector<std::uint8_t>& data);

    /**
     * @brief Read data from the registers, maps the register data to the
     * requested read region
     *
     * @param address The start address of the read region
     * @param size The size of the read region in 16-bit words
     * @return std::vector<std::uint8_t> The data read from the registers in big-endian
     * (can be converted to uint16 registers in big-endian order)
     */
    std::vector<std::uint8_t> on_read(std::uint16_t address, std::uint16_t size);

    /**
     * @brief Add a subregistry to the registry
     *
     * @param registry the shared pointer to the subregistry
     */
    void add(std::shared_ptr<ComplexRegisterSubregistry> registry);
};

} // namespace registry
} // namespace registers
} // namespace modbus
