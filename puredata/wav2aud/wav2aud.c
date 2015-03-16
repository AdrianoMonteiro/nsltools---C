/* IMPLEMENATION OF WAV2AUD FOR PURE DATA.
 * 
 * This is a C version of wav2aud function originaly written for MATLAB:
 *  
 * WAV2AUD computes the auditory spectrogram for an acoustic waveform.
 * This function takes the advantage of IIR filter's fast performance
 * which not only reduces the computaion but also saves remarkable
 * memory space.
 *
 * MATLAB VERSIONS:
 *
 * Author: Powen Ru (powen@isr.umd.edu), NSL, UMD
 * v1.00: 01-Jun-97
 * 
 * Revision: Taishih Chi (tschi@isr.umd.edu), NSL, UMD
 * v1.10: 04-Sep-98, add Kuansan's filter (as FIR filter)
 * 
 * Revision: Taishih Chi (tschi@isr.umd.edu), NSL, UMD
 * v2.00: 24-Jul-01, add hair cell membrane (lowpass) function
 * 
 * Revision: Taishih Chi (tschi@isr.umd.edu), NSL, UMD
 * v2.10: 04-Apr-04, remove FIR filtering option (see wav2aud_fir.m) 
 *
 * C-PD VERSION
 * 
 * C(Pure data)version: Adriano Monteiro (monteiro.adc@gmail.com), NICS, UNICAMP
 * v3.00 : 11-jan-2013,  implentation in C language as external for Pure Data. 
 *  
 */
 
#include "m_pd.h"
#include "../../../nsltools/nsltools.h"


static t_class *wav2aud_class;

typedef struct _wav2aud {
        t_object x_obj;
        
        
        // leitura da array
        t_word *x_vec;
        t_symbol *x_arrayname;
    	int x_array_points;
    	                
        // buffer
        float *v5; //v5[frame][channel]
        t_atom *frame_out;
        int N_frames; // N no codigo matlab - number of frames
        
        // outlets
        t_outlet *x_list;
        t_outlet *number_of_frames;
               
        wav2aud_param_t param;

        
}t_wav2aud;


/******************* BANG MESSAGE ***************************************/

static void wav2aud_bang(t_wav2aud *x) {

	int i, pad_size;
	double *wav;
	t_garray *a;
	//wav2aud_param_t param;

	// access array
	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
        pd_error(x, "%s: no such array", x->x_arrayname->s_name);
    else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
    	pd_error(x, "%s: bad template for tabletool", x->x_arrayname->s_name);
	else {	
		
		x->N_frames = ceil((t_float) x->x_array_points / x->param.L_frm); // number of frames
		pad_size = x->param.L_frm * x->N_frames;	// zero-padded: array's new size
		
		wav = (double *)getbytes(pad_size*sizeof(double));			// buffer for array data (original signal)
		x->v5 = (float *)getbytes((x->N_frames * M-1) * sizeof(float));// allocates final data buffer
		
		for(i=0; i < pad_size; i++) {
			if(i < x->x_array_points) wav[i] = x->x_vec[i].w_float; // copy array
			else wav[i] = 0; //zero padding
			}
		
		// return number of frames.
		outlet_float(x->number_of_frames, x->N_frames);
		
		// extract auditory spectrogram
		nsltools_wav2aud(x->v5, wav, pad_size, x->N_frames, x->param);
			
		// free local memory
		freebytes(wav, pad_size*sizeof(double));
	}
	
  
}


/****************************** METHODS FOR DATA ACCESS  **********************************/


static void wav2aud_getparam(t_wav2aud *x){
	
	//wav2aud_param_t *param;
	post("fac %f, shft %f, alpha %f, beta %f, L_frm %f", x->param.fac, x->param.shft, x->param.alpha, x->param.beta, x->param.L_frm);
}

static void wav2aud_get_frame(t_wav2aud *x, t_floatarg index){
	
	if( (index >= 0) && (index < x->N_frames)){
		int i, frame;
		frame = (int)((M-1)*index);
		for(i=0; i < M-1; i++) SETFLOAT(x->frame_out+i, x->v5[frame+i]); // copy collum
	
		outlet_list(x->x_obj.ob_outlet, gensym("list"), M-1,  x->frame_out);	
	}
	
}

static void wav2aud_get(t_wav2aud *x, t_floatarg frame, t_floatarg channel){
	
	if ( (frame>=0) && (frame < x->N_frames) && (channel>=0) && (channel < M-1)){
		int i, f;
		f = (int)(frame*(M-1));
		i =(int)channel;
		post("value = %F", x->v5[f+i]);
	}
	
}
		
/************************** OBJECT CREATION AND SETUP *************************************/

static void wav2aud_free(t_wav2aud *x){
	
	
	freebytes(x->v5, sizeof(float)*(x->N_frames*M-1));
	freebytes(x->frame_out,M * sizeof(t_atom));
	
}

static void *wav2aud_new(t_symbol *s, int argc, t_atom *argv){
   t_wav2aud *x = (t_wav2aud *)pd_new(wav2aud_class);
   
	wav2aud_param_t *prp = &x->param; 
	
	int i;  
	float *parameters;
    if(argc > 1){
   	    parameters = (float *)getbytes((argc -1) * sizeof(float));	
	    for(i = 1; i < argc; i++){
		  parameters[i-1] = atom_getfloat(argv+i);
	    }
		// get parameters
		nsltools_get_param(prp, argc-1, parameters); 
		freebytes(parameters, (argc-1) * sizeof(float));
	}else {
		nsltools_get_param(prp, 0, NULL); 
	}

    	 	
 // acesso a array copiado do tabletool do brent 	
	t_garray *a;
   	if(atom_getsymbol(argv)) {
  		x->x_arrayname = atom_getsymbol(argv);
  		if(!(a = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
			pd_error(x, "%s: no such array", x->x_arrayname->s_name);
  		else if(!garray_getfloatwords(a, &x->x_array_points, &x->x_vec))
			pd_error(x, "%s: bad template for wav2aud", x->x_arrayname->s_name);	
  	}else
  		post("WARNING: no array specified.");
  		
	// allocate memory for frame output as list
	x->frame_out = (t_atom *)getbytes(M * sizeof(t_atom));	
  		
	// cria os outlets
  	x->x_list = outlet_new(&x->x_obj, gensym("list")); 
  	x->number_of_frames = outlet_new(&x->x_obj, gensym("float"));
     
     return (void *) x;
}

    
void wav2aud_setup(void) {
     wav2aud_class = class_new(gensym("wav2aud"), (t_newmethod)wav2aud_new, 
     (t_method)wav2aud_free, sizeof(t_wav2aud), CLASS_DEFAULT, A_GIMME, 0);
      
    class_addbang(wav2aud_class, wav2aud_bang);
    //class_addbang(wav2aud_class, wav2aud_getparam);
    class_addmethod(wav2aud_class, (t_method)wav2aud_get_frame, gensym("get_frame"),A_DEFFLOAT, 0);
    class_addmethod(wav2aud_class, (t_method)wav2aud_get, gensym("get"),A_DEFFLOAT, A_DEFFLOAT, 0);
 
}

