#ifndef RFQUACK_PROJECT_PACKETREPEATERMODULE_H
#define RFQUACK_PROJECT_PACKETREPEATERMODULE_H

#include "../RFQModule.h"
#include "../../rfquack_common.h"
#include "../../rfquack_radio.h"

extern RFQRadio *rfqRadio; // Bridge between RFQuack and radio drivers.

class PacketRepeaterModule : public RFQModule, public AfterPacketReceived {
public:
    PacketRepeaterModule() : RFQModule("packet_repeater") {}

    void onInit() override {
      // Nothing to do :)
    }

    bool afterPacketReceived(rfquack_Packet &pkt, rfquack_WhichRadio whichRadio) override {
      pkt.repeat = repeat;
      pkt.has_repeat = true;

      // Set radio in TX mode
      if (rfqRadio->getMode(repeatRadio) != rfquack_Mode_TX) {
        rfqRadio->setMode(rfquack_Mode_TX, repeatRadio);
        RFQUACK_LOG_TRACE("Radio %d now in TX mode", repeatRadio)
      }

      // Transmit
      rfqRadio->transmit(&pkt, repeatRadio);

      return !dropAfterRepeat;
    }

    void executeUserCommand(char *verb, char **args, uint8_t argsLen, char *messagePayload,
                            unsigned int messageLen) override {
      // Handle base commands
      RFQModule::executeUserCommand(verb, args, argsLen, messagePayload, messageLen);

      // Set how many repeat:
      CMD_MATCHES_UINT("repeat", "Set how many times to repeat each packet (default: 1)", repeat)

      // Whatever to drop the packet after it's repeated, without notifying any other module:
      CMD_MATCHES_BOOL("discharge_after_repeat", "Discharge the packet after it is repeated (default: false)",
                       dropAfterRepeat)

      CMD_MATCHES_WHICHRADIO("repeat_radio", "Radio used for sending the message. (default: 0: RadioA)", repeatRadio)
    }

private:
    uint8_t repeat = 1;
    bool dropAfterRepeat = false;
    rfquack_WhichRadio repeatRadio = rfquack_WhichRadio_RadioA;
};

#endif //RFQUACK_PROJECT_PACKETREPEATERMODULE_H
