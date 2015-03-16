
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


// Remember to use a different guard symbol in each header!
#ifndef _AUDITORY_SPECTROGRAM_PLUGIN_H_
#define _AUDITORY_SPECTROGRAM_PLUGIN_H_

#include <vamp-sdk/Plugin.h>
#include "../nsltools/nsltools.h"

using std::string;
using std::vector;
using std::cerr;
using std::endl;


class AuditorySpectrogram : public Vamp::Plugin
{
public:
    AuditorySpectrogram(float inputSampleRate);
    virtual ~AuditorySpectrogram();

    string getIdentifier() const;
    string getName() const;
    string getDescription() const;
    string getMaker() const;
    int getPluginVersion() const;
    string getCopyright() const;

    InputDomain getInputDomain() const { return TimeDomain; }
    size_t getPreferredBlockSize() const;
    size_t getPreferredStepSize() const;
    //size_t getMinChannelCount() const;
    //size_t getMaxChannelCount() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(string identifier) const;
    void setParameter(string identifier, float value);

    //ProgramList getPrograms() const;
    //string getCurrentProgram() const;
    //void selectProgram(string name);

    OutputList getOutputDescriptors() const;

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

protected:

    double *m_wav;
    float  *m_v5;
    std::vector<double> m_bus;
    
    nsltools_wav2aud_param m_param;
    int m_padSize;
    int m_frames;
    float m_fac;
    float m_leaky;
    size_t m_samplerate;
    
    float m_blockSize;
    float m_type;

    
};



#endif
