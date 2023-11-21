/**
 * @file sound.h
 * @brief The sound engine that translates world positions into positonal sound
 *
 * This file describes an object, called SoundEngine, that translates sounds from
 * a world position to a positional audio sound that can be played by SDL2-mixer.
 */

#ifndef SOUND_H_
#define SOUND_H_

//incomplete types from SDL
struct musicrw;
struct _Mix_Music;
typedef struct _Mix_Music Mix_Music;
struct Mix_Chunk;

/**
 * @brief The wrapper around SDL_Mixer that plays sounds ingame.
 * 
 * This class stores and plays sounds on command into either positionally (as a
 * point in the world) or nonpositionally (such as background music).
 */
class SoundEngine
{
    public:
        SoundEngine();

        /**
         * @brief Shuts down sound system.
         *
         * Stops playing any active music, cleans up game and map sound vectors, and shuts
         * down SDL_Mixer.
         */
        void clear_sound();

        /**
         * @brief Loads the approriate sound file at the given index
         *
         * Fails silently if index is invalid.
         *
         * @param n the sound index to use
         */
        void preloadsound(int n);

        /**
         * @brief Loads the sounds for every element in the map sounds vector.
         *
         * Loads the related sound file for every valid map sound index (map entity sounds).
         */
        void preloadmapsounds(); 

        /**
         * @brief Plays a sound with the given attributes.
         * 
         *
         * @param n index of the sound to play
         * @param loc world location to play at
         * @param ent entity to associate with (overwrites location)
         * @param flags music option flags
         * @param loops number of times to loop
         * @param fade ms to fade in over
         * @param chanid music channel index to use
         * @param radius distance over which sound is audible (in size 0 cubes)
         * @param expire maximum time before the sound stops playing
         * 
         * @return the channel id or std::nullopt in case of failure.
         */
        std::optional<int> playsound(int n, const vec *loc = nullptr, extentity *ent = nullptr, int flags = 0, int loops = 0, int fade = 0, int chanid = -1, int radius = 0, int expire = -1);

        /**
         * @brief Stops playing a sound of index n in the specificied channel.
         *
         * @param n the sound index to stop playing
         * @param chanid the channel id to use
         * @param fade milliseconds to spend fading out the sound
         */
        bool stopsound(int n, int chanid, int fade);

        /**
         * @brief Immediately kills all active sounds.
         *
         * Halts all sound channels in use and frees them.
         */
        void stopsounds(); 

        /**
         * @brief Restarts and updates all sound indices.
         *
         * Pauses, reloads, and resumes playback of all active sounds. Does not play
         * sounds if on the main menu.
         */
        void updatesounds(); 

        /**
         * @brief Plays a sound with the given attributes.
         *
         * @param s name of the sound to play
         * @param loc world location to play at
         * @param ent entity to associate with (overwrites location)
         * @param flags music option flags
         * @param loops number of times to loop
         * @param fade ms to fade in over
         * @param chanid music channel index to use
         * @param radius distance over which sound is audible (in size 0 cubes)
         * @param expire maximum time before the sound stops playing
         */
        int playsoundname(const char *s, const vec *loc, int vol, int flags, int loops, int fade, int chanid, int radius, int expire);

        /**
         * @brief Starts SDL_Mixer and initializes startup sound channels.
         *
         * Fails and disables sound output if SDL is v2.0.6.
         */
        void initsound();
        
        /**
         * @brief Starts a nonpositional music track.
         * 
         * @param name (inc. path) of the track to play
         * @param cmd the command to run (in CubeScript) at the end of the track
         */
        void startmusic(char *name, char *cmd);
        
        /**
         * @brief Resets the sound subsystem.
         */
        void resetsound();

        /**
         * @brief Sets whether stereo audio is enabled.
         * 
         * @param on if true, enables stereo; if false, disables it
         */
        void setstereo(const int * const on);
        
        /**
         * @brief Returns whether stereo or mono is enabled.
         * 
         * The return is in CubeScript friendly `int` form.
         * 
         * @return 1 if stereo enabled, 0 if mono enabled
         */
        int getstereo();

        /**
         * @brief Sets the maximum radius to play sounds from.
         * 
         * The distance is in cube units ( =12.5cm).
         * 
         * @param dist the distance to render from
         */
        void setmaxradius(const int * const dist);
        
        /**
         * @brief Returns the maximum sound play radius.
         * 
         * The distance is in cube units ( =12.5cm).
         * 
         * @return the maximum distance at which sounds will be played
         */
        int getmaxradius();

        /**
         * @brief Sets the number of sound channels to use.
         * 
         * Default value is 32; bounds limited to 1..128.
         * 
         * @param val the number of channels to use
         */
        void setsoundchans(const int * const val);
        
        /**
         * @brief Returns the number of sound channels in use.
         * 
         * @return the number of sound channels in use (1..128)
         */
        int getsoundchans();

        /**
         * @brief Sets the map sound volume.
         * 
         * Bounds limited to 0..255.
         * 
         * @param vol the volume level for map sounds
         */
        void setsoundvol(const int * const vol);
        
        /**
         * @brief Returns the current sound volume.
         * 
         * @return the current volume for map sounds (0..255)
         */
        int getsoundvol();

        /**
         * @brief Sets the music sound volume.
         * 
         * Bounds limited to 0..255.
         * 
         * @param vol the volume level for music tracks
         */
        void setmusicvol(const int * const musicvol);

        /**
         * @brief Returns the current music volume.
         * 
         * @return the current volume for music (0..255)
         */
        int getmusicvol();

        /**
         * @brief Sends a list of audio subsystems for SDL to bind to.
         * 
         * By default, the standard Linux and Windows audio systems are set.
         * 
         * @param f a list of space-separated names for SDL to attempt to bind
         */
        void setaudiodriver(char * f);

        /**
         * @brief Sets whether the sound engine is enabled.
         * 
         * @param on whether sound is enabled or not
         */
        void setsound(const int * const on);
        
        /**
         * @brief Returns whether sound is on.
         * 
         * @return 1 if sound is on, 0 otherwise
         */
        int getsound();

        /**
         * @brief Adds a sound to the sound list at a given volume
         * 
         * @param name the path to the sound
         * @param vol the volume of the sound (0..255)
         */
        void registersound(char *name, int *vol);
        
        /**
         * @brief Adds a sound to the list of map sounds
         * 
         * @param name the path to the sound
         * @param vol the volume of the sound (0..255)
         * @param maxuses the number of uses before the sound won't play
         */
        void mapsound(char *name, int *vol, int *maxuses);
        
        /**
         * @brief Adds an alternative game sound.
         * 
         * @param the name of the sound to add
         * @param vol the volume of the sound (0..255)
         */
        void altsound(char *name, int *vol);

        /**
         * @brief Adds an alternative map sound.
         * 
         * @param the name of the sound to add
         * @param vol the volume of the sound (0..255)
         */
        void altmapsound(char *name, int *vol);
        
        /**
         * @brief Returns to the console the number of game sounds loaded
         * 
         * @return (to console) the number of game sounds
         */
        void numsounds();
        
        /**
         * @brief Returns to the console the number of map sounds loaded.
         * 
         * @return (to console) the number of map sounds
         */
        void nummapsounds();
        
        /**
         * @brief Resets the loaded game sounds.
         * 
         * This clears the list of game (e.g. hud weapon) sounds loaded into the
         * sound engine.
         */
        void soundreset();
        
        /**
         * @brief Resets the loaded map sounds.
         * 
         * This clears the list of map environment sounds loaded into the
         * sound engine.
         */
        void mapsoundreset();

    private:
        void stopmapsound(extentity *e);

        bool stereo = true;

        int maxsoundradius = 340;

        //free channels that are not playing sounds
        void reclaimchannels();
        void syncchannels();
        bool minimizedsounds = false;
        int getminimizedsounds();
        void setminimizedsounds(int minimize);

        void stopmapsounds();
        //check map entities to see what sounds need to be played because of them
        void checkmapsounds();

        //number of sounds before the game will refuse to play another sound (with `playsound()`);
        //set to 0 to disable checking (0 does not set no sounds to be playable)
        int maxsoundsatonce = 7;
        int getmaxsoundsatonce();
        void setmaxsoundsatonce(const int * const num);
        int debugsound = 0;

        bool nosound = true;

        struct SoundSample
        {
            SoundEngine * const parent;
            std::string name;
            Mix_Chunk *chunk;

            SoundSample(SoundEngine& p);
            ~SoundSample();

            void cleanup();

            bool load(const char *dir);
        };

        struct soundslot
        {
            SoundSample *sample;
            int volume;
        };

        struct SoundConfig
        {
            int slots, numslots;
            int maxuses;

            bool hasslot(const soundslot *p, const std::vector<soundslot> &v) const;

            int chooseslot(int flags) const;
        };

        //sound channel object that is allocated to every sound emitter in use
        //(entities, players, weapons, etc.)
        //defined in world coordinates, and position mixing is done for the player dynamically
        class SoundChannel
        {
            public:
                bool inuse;
                soundslot *slot;
                extentity *ent;
                int volume;

                SoundChannel(int id, SoundEngine& p);

                bool updatechannel();
                void syncchannel();
                void setupchannel(int n, soundslot *slot, const vec *loc, extentity *ent, int flags, int radius);

                void setloc(const vec& newloc);

                bool hasloc() const;

                void clearloc();
                void reset();
            private:
                SoundEngine * const parent;
                int id;
                int radius, pan, flags;
                bool dirty;
                vec loc;
        };

        std::vector<SoundChannel> channels;
        int maxchannels = 0;

        int soundvol = 255;

        int musicvol = 60;

        bool shouldinitaudio = true;

    #ifdef WIN32
        #define AUDIODRIVER "directsound winmm"
    #else
        #define AUDIODRIVER "pulseaudio alsa arts esd jack pipewire dsp"
    #endif

        std::string audiodriver = AUDIODRIVER;
        bool sound;

        //# of sound channels (not physical output channels, but individual sound samples in use, such as weaps and light ents)
        int soundchans = 32;
        //max sound frequency (44.1KHz = CD)
        int soundfreq = 44100;
        //length of sound buffer in milliseconds
        int soundbufferlen = 1024;

        bool initaudio();

        struct SoundType
        {
            SoundEngine * const parent;
            std::map<std::string, SoundSample> samples;
            std::vector<soundslot> slots;
            std::vector<SoundConfig> configs;
            const char *dir;
            SoundType(const char *dir, SoundEngine& p);
            std::optional<int> findsound(const char *name, int vol);
            int addslot(const char *name, int vol);
            int addsound(const char *name, int vol, int maxuses = 0);
            void addalt(const char *name, int vol);
            void clear();
            void reset();
            void cleanupsamples();
            void cleanup();
            void preloadsound(int n);
            bool playing(const SoundChannel &chan, const SoundConfig &config) const;
        };
        SoundType gamesounds;
        SoundType mapsounds;

        //free all channels
        void resetchannels();

        void musicdone();
        Mix_Music *loadmusic(const char *name);
        Mix_Chunk *loadwav(const char *name);

        //creates a new SoundChannel object with passed properties
        SoundChannel &newchannel(int n, soundslot *slot, const vec *loc = nullptr, extentity *ent = nullptr, int flags = 0, int radius = 0);

        //sets a channel as not being in use
        void freechannel(int n);

        void stopchannels();

        void setmusicvol(int musicvol);

        char *musicfile    = nullptr,
             *musicdonecmd = nullptr;

        Mix_Music *music    = nullptr;
        SDL_RWops *musicrw  = nullptr;
        stream *musicstream = nullptr;

        void stopmusic();
};
#endif
