/*
 * Copyright (c) 2011, Intel Corporation.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "irq.h"
#include "globals.h"


IRQ::IRQ()
{
}


IRQ::~IRQ()
{
}


bool
IRQ::VerifyAnySchemeSpecifyNum(uint16_t numIrqsDesire, enum nvme_irq_type &irq)
{
    bool capable;
    uint16_t numIrqsSupported;
    irq = INT_NONE;     // assume DUT doesn't support request


    // Can we satisfy the request using the MSI-X IRQ scheme?
    if (gCtrlrConfig->IsMSIXCapable(capable, numIrqsSupported) == false)
        throw exception();
    else if (capable) {
        if (numIrqsDesire > numIrqsSupported) {
            LOG_NRM("Desire %d IRQs, but DUT only supports %d MSI-X IRQ's",
                numIrqsDesire, numIrqsSupported);
        } else {
            irq = INT_MSIX;
            return true;
        }
    }

    // Can we satisfy the request using the MSI IRQ scheme?
    if (gCtrlrConfig->IsMSICapable(capable, numIrqsSupported) == false)
        throw exception();
    else if (capable) {
        if (numIrqsDesire > numIrqsSupported) {
            LOG_NRM("Desire %d IRQs, but DUT only supports %d MSI IRQ's",
                numIrqsDesire, numIrqsSupported);
        } else {
            irq = (numIrqsSupported == 1) ? INT_MSI_SINGLE : INT_MSI_MULTI;
            return true;
        }
    }
    LOG_ERR("DUT not reporting any IRQ scheme satisfying %d IRQ's",
        numIrqsDesire);
    return false;
}


void
IRQ::SetAnySchemeSpecifyNum(uint16_t numIrqsDesire)
{
    enum nvme_irq_type irqScheme;

    if (VerifyAnySchemeSpecifyNum(numIrqsDesire, irqScheme) == false)
        throw exception();

    if (gCtrlrConfig->SetIrqScheme(irqScheme, numIrqsDesire) == false)
        throw exception();
}
