bool charIsDelim(char c, char *delim){

    char * pch = delim;
    pch=strchr(delim,c);
    
    if((pch-delim)>=0){
        return 1;
    }else{
        return 0;
    }
}


int parseString(char * s, int len,  char * delim, int maxTokens, char ** argv){
    int argc ;
       
    int i;
    bool removeWhiteSpace = 1;
    for(i=0;i<len;i++){

        if(  charIsDelim(s[i],delim)   && (removeWhiteSpace)  ){
            s[i] = 0;
        }

        if(s[i] == '"' ){
            s[i] = 0;
            removeWhiteSpace = !removeWhiteSpace;
        }
        
    }
    
    int start = 0;
    argc = 0;
    for(i=0;i<len;i++){
        if(s[i] != 0){
            start = i;
            argv[argc++] = &s[i];
            break;
        }
    }
    
    for(i=start+1;i<len;i++){
        if( (s[i] != 0) && (s[i-1] == 0)  ){
            argv[argc++] = &s[i];
            if(argc > maxTokens){
                break;
            }
        }
    }
    
    
    return argc;
}
