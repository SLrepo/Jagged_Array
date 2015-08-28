template <class T> class JaggedArray{
	public:
		//constructor, destructor, assignment operator
		JaggedArray() { this -> create();}
		JaggedArray(const int b){this->create(b);}
		JaggedArray (const JaggedArray& ja){copy(ja);}
		JaggedArray& operator=(const JaggedArray& ja);
		~JaggedArray(){this->del();}

		//member function and other operator
		unsigned int numElements() const {return num_elements;}
		unsigned int numBins() const {return num_bins;}
		unsigned int numElementsInBin(const unsigned int& b) const; 
		T& getElement (const unsigned int& b, const unsigned int& locInBin) const;
		bool isPacked() const {return ispack;}
		void addElement(const unsigned int& b, const T& val);
		void removeElement(const unsigned int& b, const unsigned int& locInBin);
		void clear(){this->del();}
		void print() const;
		void pack();
		void unpack();
	private:
		//private member functions
		void create();
		void create(const int n);
		void copy(const JaggedArray& ja);
		void del();

		//representations
		unsigned int num_elements;
		unsigned int num_bins;
		unsigned int * counts; 
		unsigned int * offsets;
		bool ispack;
		T* packed_values;
		T** unpacked_values;
};
template <class T> void JaggedArray<T>::create(){
	num_elements=0;
	num_bins=0;
	ispack=false;
	counts=NULL;
	offsets=NULL;
	packed_values=NULL;
	unpacked_values=NULL;
}
template <class T> void JaggedArray<T>::create(const int b){
	num_elements=0;
	num_bins=b;
	ispack=false;
	counts=new unsigned int[b];
	unpacked_values= new T*[b];
	for (unsigned int i=0; i<b; i++){
		counts[i]=0;
		unpacked_values[i]=NULL;
	}
	offsets=NULL;
	packed_values=NULL;
	
}
template <class T> void JaggedArray<T>::copy(const JaggedArray<T>& ja){
	num_elements=ja.numElements();
	num_bins=ja.numBins();
	if (ja.ispack==false){
		ispack=false;
		counts=new unsigned int[ja.numBins()];
		offsets=NULL;
		packed_values=NULL;
		unpacked_values=new T*[num_bins];
        for (unsigned int i=0; i<num_bins; i++){
        	unpacked_values[i]= new T[ja.numElementsInBin(i)];
        	counts[i]=ja.counts[i];
        	for (unsigned int j=0; j<ja.numElementsInBin(i);j++){
        		unpacked_values[i][j]=ja.getElement(i,j);
        	}
        }
	}
	if(ja.ispack==true){
		ispack=true;
		counts=NULL;
		unpacked_values=NULL;
		offsets=new unsigned int[ja.numBins()];
		packed_values=new T[num_elements];
		for (unsigned int i=0; i<num_elements;i++){
			packed_values[i]=ja.packed_values[i];
		}
		for (unsigned int i=0; i<num_bins; i++){
			offsets[i]=ja.offsets[i];
		}
	}
}

template <class T> JaggedArray<T>& JaggedArray<T>::operator=(const JaggedArray<T>& ja){
	if(this!=&ja){
		num_bins=ja.num_bins;
		num_elements=ja.num_elements;
		this->del();
		copy(ja);
	}
    return *this;
}
template <class T> unsigned int JaggedArray<T>::numElementsInBin(const unsigned int& b) const{
    if(counts==NULL && offsets==NULL){
    	return 0;
    }
    else if(ispack==false)
    	return counts[b];
    else 
    	if(b==num_bins-1)
    		return num_elements-offsets[num_bins-1];
    	else
    		return offsets[b+1]-offsets[b];
}

template <class T> void JaggedArray<T>::pack(){
	if (ispack==true){
		std::cerr << "this JaggedArray is already packed."<<std::endl;
		exit(1);
	}
	offsets=new unsigned int[num_bins];
	offsets[0]=0;
	for (unsigned int i=1; i<num_bins; i++){
		offsets[i]=counts[i-1]+offsets[i-1];
	}
    //counts=NULL;
    unsigned int e=0;
    packed_values=new T[num_elements];
    for (unsigned int i=0; i<num_bins; i++){
    	for (unsigned int j=0; j<counts[i]; j++){
    		packed_values[e]=unpacked_values[i][j];
    		e++;
    	}
    	delete [] unpacked_values[i];
    }

    delete [] unpacked_values;
    delete [] counts;
    counts=NULL;
    unpacked_values=NULL;
    ispack=true;
}

template <class T> void JaggedArray<T>::unpack(){
	if (ispack==false){
		std::cerr << "the JaggedArray is already unpacked." << std::endl;
		exit(1);
	}
	counts=new unsigned int[num_bins];
	for (unsigned int i=0; i<num_bins-1; i++){
		counts[i]=offsets[i+1]-offsets[i];
	}
	counts[num_bins-1]=num_elements-offsets[num_bins-1];
    unpacked_values=new T*[num_bins];
    unsigned int e=0;
    for (unsigned int i=0; i<num_bins; i++){
    	unpacked_values[i] = new T[counts[i]];
    	for (unsigned int j=0; j<counts[i]; j++){
    		unpacked_values[i][j]=packed_values[e];
    		e++;
    	}
    }
    delete [] packed_values;
    delete [] offsets;
    offsets=NULL;
    packed_values=NULL;
    ispack=false;
}

template <class T> void JaggedArray<T>::del(){
	if (ispack==true){
		delete [] offsets;
		offsets=NULL;
		delete [] packed_values;
		packed_values=NULL;
		//packed values=NULL;
	}
	else{
		for (unsigned int i=0; i<num_bins;i++){
			delete [] unpacked_values[i];
			unpacked_values[i]=NULL;
			//counts[i]=0;
		}
		delete [] counts;
		counts=NULL;
		delete [] unpacked_values;
	}
	num_elements=0;
	num_bins=0;
	ispack=false;
	unpacked_values=NULL;
}

template <class T> void JaggedArray<T>::addElement(const unsigned int& b, const T& val){
    if (ispack==true){
    	std::cerr << "first unpackd the JaggedArray. then add value" << std::endl;
    	exit(1);
    }
    if (ispack==false){
    	T* temp=new T[counts[b]+1];
    	for (unsigned int i=0; i<counts[b];i++){
    		temp[i]=unpacked_values[b][i];
    	}
    	temp[counts[b]]=val;
    	if (unpacked_values[b]!=NULL){
    	    delete [] unpacked_values[b];
    	}
    	unpacked_values[b]=temp;
    	num_elements++;
    	counts[b]++;
    }
}

template <class T> void JaggedArray<T>::removeElement(const unsigned int& b, const unsigned int& locInBin){
	if (ispack==true){
    	std::cerr << "first unpackd the JaggedArray. then remove value" << std::endl;
    	exit(1);
    }
    T* temp=new T[counts[b]-1];
    unsigned int e=0;
    for (unsigned int i=0; i< counts[b]; i++){
    	if (i!=locInBin){
    		temp[e]=unpacked_values[b][i];
    		e++;
    	}
    }
    delete [] unpacked_values[b];
    unpacked_values[b]=temp;
    num_elements--;
    counts[b]--;
}

template <class T> void JaggedArray<T>::print() const {
    std::cout<< "the num of elements in the JaggedArray is "<<num_elements<<std::endl;
    std::cout<< "the num of bins in the JaggedArray is " << num_bins<<std::endl;

    if(ispack==false){
    	for (unsigned int i=0; i<num_bins; i++){
    		std::cout << "the num of elements in bin "<< i << " is "<<counts[i]<<std::endl;
    		for (unsigned int j=0; j<counts[i]; j++){
    			std::cout << "the " << j<<" element is " <<unpacked_values[i][j]<<std::endl;
    		}
    	}
    	//std::cout << "test " << 'a'<<std::endl;
    }

    if(ispack==true){
    	for (unsigned int i=0;i<num_elements; i++){
    		std::cout <<"the "<<i<< " element is "<<packed_values[i]<<std::endl;
    	}
    	for (unsigned int i=0; i<num_bins;i++){
    		std::cout << "the "<<i<<" bin starts from the "<< offsets[i]<<" element"<<std::endl;
    	}
    }
}

template <class T> T& JaggedArray<T>::getElement(const unsigned int& b, const unsigned int& locInBin) const{
	if (ispack==false){
		return unpacked_values[b][locInBin];
	}
	else{
		return packed_values[offsets[b]+locInBin];
	}
} 





























