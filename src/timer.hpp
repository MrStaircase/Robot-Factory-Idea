class Timer{
private:
    unsigned long _last_triggered;
    
public:
    static constexpr unsigned long unsigned_long_max = 0UL - 1UL;

    void initialize(unsigned long current_time){
        _last_triggered = current_time;
    }

    bool check_trigger(unsigned long current_time, unsigned delay){
        bool overflow = _last_triggered > current_time;

        unsigned long elapsed_time =
            !overflow ?
            current_time - _last_triggered :
            unsigned_long_max - _last_triggered + current_time + 1;

        if(elapsed_time >= delay){
            _last_triggered += delay;
            return true;
        }
        
        return false;
    }

    void delay_timer(unsigned long delay){
        _last_triggered += delay;
    }
};