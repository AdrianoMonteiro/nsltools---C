
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


#include "AuditorySpectrogram.h"

using std::string;
using std::vector;
using std::cerr;
using std::endl;


AuditorySpectrogram::AuditorySpectrogram(float inputSampleRate) :
   Vamp::Plugin(inputSampleRate),
  
   m_wav(0),
   m_v5(0),
   m_bus(0),
   //m_param(0),
   m_padSize(0),
   m_frames(0),
   m_fac(0.1),
   m_leaky(0),
   m_samplerate(inputSampleRate),
   m_blockSize(0),
   m_type(0)

   
{
}

AuditorySpectrogram::~AuditorySpectrogram()
{
	//free wav;
	//free v5;// deleta vetores 
	
}

string
AuditorySpectrogram::getIdentifier() const
{
    return "AuditorySpectrogram";
}

string
AuditorySpectrogram::getName() const
{
    return "Auditory Spectrogram";
}

string
AuditorySpectrogram::getDescription() const
{
    // Return something helpful here!
    return "convert sound signal to auditory spectrogram";
}

string
AuditorySpectrogram::getMaker() const
{
    // Your name here
    return "Adriano Monteiro";
}

int
AuditorySpectrogram::getPluginVersion() const
{
    // Increment this each time you release a version that behaves
    // differently from the previous one
    return 1;
}

string
AuditorySpectrogram::getCopyright() const
{
    // This function is not ideally named.  It does not necessarily
    // need to say who made the plugin -- getMaker does that -- but it
    // should indicate the terms under which it is distributed.  For
    // example, "Copyright (year). All Rights Reserved", or "GPL"
    return "coprigth (c) Adriano Monteiro 2013";
}

/*   Inicialização feita no .h
MyPlugin::InputDomain
MyPlugin::getInputDomain() const
{
    return TimeDomain;
}
*/

size_t
AuditorySpectrogram::getPreferredBlockSize() const
{
    return 0; // 0 means "I can handle any block size"
}

size_t 
AuditorySpectrogram::getPreferredStepSize() const
{
    return 0; // 0 means "anything sensible"; in practice this
              // means the same as the block size for TimeDomain
              // plugins, or half of it for FrequencyDomain plugins
}

/*
size_t
AuditorySpectrogram::getMinChannelCount() const
{
    return 1;
}

size_t
AuditorySpectrogram::getMaxChannelCount() const
{
    return 1;
}

*/

AuditorySpectrogram::ParameterList
AuditorySpectrogram::getParameterDescriptors() const
{
    ParameterList list;

    // If the plugin has no adjustable parameters, return an empty
    // list here (and there's no need to provide implementations of
    // getParameter and setParameter in that case either).

    // Note that it is your responsibility to make sure the parameters
    // start off having their default values (e.g. in the constructor
    // above).  The host needs to know the default value so it can do
    // things like provide a "reset to default" function, but it will
    // not explicitly set your parameters to their defaults for you if
    // they have not changed in the mean time.
   
	ParameterDescriptor leaky;
    leaky.identifier = "leaky_integrator";
    leaky.name = "leaky integrator";
    leaky.description = " constant time for leaky integrator";
    leaky.unit = "ms";
    leaky.minValue = 0;
    leaky.maxValue = 30;
    leaky.defaultValue = 8;
    leaky.isQuantized = false;
    list.push_back(leaky);
     
    
    ParameterDescriptor fac_type;
    fac_type.identifier = "Type_of_fac";
    fac_type.name = "Hair Cell Non-linear Compression";
    fac_type.description = "a monotonic increasing function which simulates hair cell nonlinearity";
    fac_type.unit = "";
    fac_type.minValue = 0;
    fac_type.maxValue = 4;
    fac_type.defaultValue = 1;
    fac_type.isQuantized = true;
    fac_type.quantizeStep = 1;
    fac_type.valueNames.push_back("Linear");
    fac_type.valueNames.push_back("Half-wave Rectifier");
    fac_type.valueNames.push_back("Hard Limiter");
	fac_type.valueNames.push_back("Half Duration Regulator");
    fac_type.valueNames.push_back("Non-linear: Transister-like Function");
    list.push_back(fac_type);
   
    ParameterDescriptor fac;
    fac.identifier = "nonlinear_factor";
    fac.name = "Non-linear factor";
    fac.description = "";
    fac.unit = "ms";
    fac.minValue = 0.0001;
    fac.maxValue = 20;
    fac.defaultValue = 0.1;
    fac.isQuantized = false;
    list.push_back(fac);
    

    return list;
}

float
AuditorySpectrogram::getParameter(string identifier) const
{
    if (identifier == "Type_of_fac") {
		return m_type;		
    } else 
   if (identifier == "nonlinear_factor") {
		return m_fac;
    } else if (identifier == "leaky_integrator") {
        return m_leaky;
    }
    
    return 0.0f;
}

void
AuditorySpectrogram::setParameter(string identifier, float value) 
{
     if (identifier == "Type_of_fac") {
		m_type = value;
    } else if (identifier == "nonlinear_factor") {
        m_fac  = value;
    } else if (identifier == "leaky_integrator") {
         m_leaky = value;
    }
}

/*
MyPlugin::ProgramList
MyPlugin::getPrograms() const
{
    ProgramList list;

    // If you have no programs, return an empty list (or simply don't
    // implement this function or getCurrentProgram/selectProgram)

    return list;
}


string
MyPlugin::getCurrentProgram() const
{
    return ""; // no programs
}

void
MyPlugin::selectProgram(string name)
{
}

 */
 
 
AuditorySpectrogram::OutputList
AuditorySpectrogram::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.

    OutputDescriptor d;
    d.identifier = "accum";
    d.name = "Accum block size";
    d.description = "";
    d.unit = "";
    d.hasFixedBinCount = true;
    d.binCount = M-1;
    d.hasKnownExtents = false;
    d.isQuantized = false;
    d.sampleType = OutputDescriptor::FixedSampleRate;
    d.sampleRate = 1.0 / (m_blockSize/m_samplerate);
    d.hasDuration = false;
    list.push_back(d);

    return list;
}

bool
AuditorySpectrogram::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() ||
	channels > getMaxChannelCount()) return false;
    
    m_blockSize = float(blockSize);
    
    wav2aud_param_t *prp = &m_param; 
    float parameters[4];
    float blockTime;
    
    blockTime = (m_blockSize/m_samplerate)*1000;
    
    switch( int(m_type)){
		case 0: m_fac = -2; break;
		case 1: m_fac = -1; break;
		case 2: m_fac =  0; break;
		case 3: m_fac = -3; break;
	}
    
    parameters[0] = blockTime;
    parameters[1] = m_leaky;
    parameters[2] = m_fac;
    parameters[3] = m_samplerate;
           
    nsltools_get_param(prp, 4, parameters); 
    
   
    return true;
}

void
AuditorySpectrogram::reset()
{
	m_frames = 0;
	m_padSize = 0;
    // Clear buffers, reset stored values, etc
}

AuditorySpectrogram::FeatureSet
AuditorySpectrogram::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{
    
    int j; 
   
    for(j = 0;  j < m_blockSize; j++){
		m_bus.push_back(inputBuffers[0][j]);
	}
	

	return FeatureSet();
	
}

AuditorySpectrogram::FeatureSet
AuditorySpectrogram::getRemainingFeatures()
{ 
	FeatureSet returnFeatures;
	int  size, i, j, frame_idx;
	
	size = int(m_bus.size());
	
	m_frames = ceil((float) size / m_blockSize); // number of frames
	m_padSize = m_blockSize * m_frames;	// zero-padded: array's new size
	
	m_wav = new double[m_padSize];
	m_v5 = new float[m_frames * (M-1)];// allocates final data buffer
		
	for(i=0; i < m_padSize; i++) {
		if(i < size) m_wav[i] = m_bus[i]; // copy array
		else m_wav[i] = 0; //zero padding
	}
	
	nsltools_wav2aud(m_v5, m_wav, m_padSize, m_frames, m_param);

	for (i = 0; i < m_frames; i++){	
		Feature feature;
		feature.hasTimestamp = false;
		feature.timestamp = Vamp::RealTime(0,i);
		feature.label = "pontos";
    
		for(j = 0; j < M-1; j++){
			frame_idx = i * (M-1);
			feature.values.push_back(m_v5[frame_idx+j]);
		}
		returnFeatures[0].push_back(feature);
	
	}
		
	return returnFeatures;
	delete[] m_wav;
	delete[] m_v5;
	
	
}

