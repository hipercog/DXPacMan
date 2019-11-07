// DGD1 Survey processor copyright 2005 Neil Bundy
// Update and data export code c.2007 Ben Cowley

function gettype(f)
{
	// initialise the array used as a results counter
	var type = [0,0,0,0];
	// initialise the array which will fill in the results
	var result = ["Not Significantly","Marginally","Moderately","Strongly","Dominantly"];
    
    /* @Ben
        Check each radio button group for each Q, add values to type[] array 
        */
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q1'][i].checked )
	        type[0] += i;

	for(var i = 0; i < 5; i++)
	    if( f.elements['Q2'][i].checked )
	        type[1] += i;
	        
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q3'][i].checked )
	        type[2] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q4'][i].checked )
	        type[3] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q5'][i].checked )
	        type[0] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q6'][i].checked )
	        type[1] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q7'][i].checked )
	        type[2] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q8'][i].checked )
	        type[3] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q9'][i].checked )
	        type[0] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q10'][i].checked )
	        type[1] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q11'][i].checked )
	        type[2] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q12'][i].checked )
	        type[3] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q13'][i].checked )
	        type[0] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q14'][i].checked )
	        type[1] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q15'][i].checked )
	        type[2] += i;
	
	for(var i = 0; i < 5; i++)
	    if( f.elements['Q16'][i].checked )
	        type[3] += i;
	        
//	alert(document.getElementById('1_' + (type[0]+1)).name);
	
	// fill a var with the result as a word: "Moderately" etc to fill in the results on the results page
	if(type[0] == 0)
	    type[0]++;
	self.Type1_word = result[Math.round(type[0] / 4)];
	if(type[1] == 0)
	    type[1]++;
    self.Type2_word = result[Math.round(type[1] / 4)];
	if(type[2] == 0)
	    type[2]++;
    self.Type3_word = result[Math.round(type[2] / 4)];
	if(type[3] == 0)
	    type[3]++;
    self.Type4_word = result[Math.round(type[3] / 4)];
	
	// fill a variable with the number of checks for each type this will make the results font size change accordingly
	self.Type1_number = Math.round(type[0]);
	self.Type2_number = Math.round(type[1]);
	self.Type3_number = Math.round(type[2]);
	self.Type4_number = Math.round(type[3]);
	
	/* @Ben
        Make the results into returnable data
        */
     var gamerType = f.elements['Part1Q1'][0].value;
     if( f.elements['Part1Q1'][1].checked )
        gamerType = f.elements['Part1Q1'][1].value;
     if( f.elements['Part1Q1'][2].checked )
        gamerType = f.elements['Part1Q1'][2].value;

     var sex = f.elements['QGender'][0].value;
     if( f.elements['QGender'][1].checked )
        sex = f.elements['QGender'][1].value;
    
    //Get the IP address of the user (script is in html page) - periods should be replaced with commas already
    var IP = new String( f.elements['ipAddr'].value );
    //If we have no IP address, save the results under a (hopefully) unique number & use time analysis to match to gameplay results
    if(IP.length < 7)
    {        
        var time = new Date();
        IP = time.getTime();
    }
    
    f.elements['Sex'].value = sex;
	f.elements['GamerType'].value = gamerType;
	f.elements['ipAddr'].value = IP;
	
	f.elements['Type1'].value = type[0];
	f.elements['Type2'].value = type[1];
	f.elements['Type3'].value = type[2];
	f.elements['Type4'].value = type[3];
	
/**/
    // open results window
	var w = window.open("DGD1_thanks.shtml", "_thanks");
    
    //Click hidden submit button in main page, for posting results    setTimeout('', 2000);
    f.elements['submit'].click();
    //setTimeout('window.open("DGD1_thanks.shtml", "_thanks")', 2000);
/**/

}
//End Code
