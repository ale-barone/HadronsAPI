#include "Utilities.hpp"

BEGIN_APIMODULE_NAMESPACE(ASink)


// SINK
std::string make_sink(Application &application, std::array<int, 3> mom){
    MSink::ScalarPoint::Par sink; 
    sink.mom = make_3mom_par(mom);
    std::string sink_name = "sinkpt_mom_" + make_3mom_name(mom);
    application.createModule<MSink::ScalarPoint>(sink_name, sink);
    return sink_name;
}


END_APIMODULE_NAMESPACE
