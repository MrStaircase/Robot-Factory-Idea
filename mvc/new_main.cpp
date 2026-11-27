#include <vector>

class Registry{};
class IData{
public:
    virtual ~IData() = default;
    virtual bool load(Registry&) = 0;
    virtual void save(const Registry&) = 0;
};
class Event{};
class IDraw{
public:
    virtual ~IDraw() = default;
    virtual void draw(const Registry&) = 0;
    virtual std::vector<Event> get_events() = 0;
};

class JsonManager{};
class SdlDrawManager{};
class SystemManager{};

class JsonData: public IData{
private:
    JsonManager& _manager;
public:
    JsonData(JsonManager& manager): _manager(manager) { }
    bool load(Registry&) override { return false; }
    void save(const Registry&) override {}
};
class SdlDraw: public IDraw{
private:
    SdlDrawManager& _manager;
public:
    SdlDraw(SdlDrawManager& manager): _manager(manager){ }
    void draw(const Registry&) override {}
    std::vector<Event> get_events() override { return {}; }
};

enum class SimulationState{CONTINUE, QUIT, FAILURE};

enum class DataRequest{NONE, LOAD, SAVE};

class Simulation{
private:
    SystemManager& _system_manager;
    
public:
    Registry registry;
    DataRequest data_request = DataRequest::NONE;

    Simulation(SystemManager& system_manager):
        _system_manager(system_manager) {}
    
    SimulationState init() { return SimulationState::CONTINUE; }
    SimulationState event(const Event&) { return SimulationState::CONTINUE; }
    SimulationState iterate() { return SimulationState::CONTINUE; }
    void quit() {}
};

class Application{
private:
    IData& _data;
    IDraw& _draw;
    Simulation& _simulation;

public:
    Application(IData& data, IDraw& draw, Simulation& simulation):
        _data(data), _draw(draw), _simulation(simulation) {}
    void run() {
        SimulationState state = _simulation.init();
        while (state == SimulationState::CONTINUE){
            for (const Event& event : _draw.get_events()){
                // FAILURE checkup in the loop are ommited for simplicity
                state = _simulation.event(event);
            }
            state = _simulation.iterate();

            if (_simulation.data_request == DataRequest::LOAD){
                _data.load(_simulation.registry);
            }
            
            if (_simulation.data_request == DataRequest::SAVE){
                _data.save(_simulation.registry);
            }

            _draw.draw(_simulation.registry);
        }
        _simulation.quit();
    }
};

int main(){
    JsonManager json_manager;
    SdlDrawManager sdl_draw_manager;
    SystemManager system_manager;

    JsonData json_data{json_manager};
    SdlDraw sdl_draw{sdl_draw_manager};

    Simulation simulation{system_manager};
    Application application{json_data, sdl_draw, simulation};

    application.run();

    return 0;
}
