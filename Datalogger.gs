// BY: Varmo Ernits
// ON: 01.12.21
// AT: github.
 
 
// Steps are valid as of 10.12.21
// 0) From Google spreadsheet - Extensions - Apps Script
// 1) Write your code
// 2) Save and give a meaningful name
// 3) Run and make sure "doPost" is selected
//    You can set a method from Run menu
// 4) When you run for the first time, it will ask 
//    for the permission. You must allow it.
//    Make sure everything is working as it should.
// 5) From Publish menu - Deploy as Web App...
//    Select a new version everytime it's published
//    Type comments next to the version
//    Execute as: "Me (your email address)"
//    MUST: Select "Anyone, even anonymous" on "Who has access to this script"
//    For the first time it will give you some prompt(s), accept it.
//    You will need the given information (url) later. This doesn't change, ever!
 
// Saving the published URL helps for later.
// https://script.google.com/macros/s/---Your-Script-ID--Goes-Here---/exec
// https://script.google.com/macros/s/---Your-Script-ID--Goes-Here---/exec?tag=test&amp;value=-1
 
// This method will be called first or hits first  
function doPost(e){
  Logger.log("--- doPost ---");
 
 var hdc1080Temp = "",
     hdc1080Hum = "",
     neCO2 = "",
     nTVOC = "",
     bme280Temp = "",
     bme280Hum = "",
     bme280Pres = "",
     
     tempIN = ""
     humidity = "",
     tempOUT = "";
  
  var parsedData;
  
  try { 
    parsedData = JSON.parse(e.postData.contents);
  } 
  catch(f){
    return ContentService.createTextOutput("Error in parsing request body: " + f.message);
  }
   
  if (parsedData !== undefined){
    // Common items first
    // data format: 0 = display value(literal), 1 = object value
    var flag = parsedData.format;
    
    if (flag === undefined){
      flag = 0;
    }
    // "appendRow" and "appendRow2" are parts of url-s through which IOT devices send data to google scripts, 
    // name as you wish, 2 different cases for 2 different devices and sheets tab.
  switch (parsedData.command) {
      case "appendRow":
      
      var dataArr = parsedData.values.split(",");
      
      hdc1080Temp = dataArr[0];
      hdc1080Hum = dataArr[1];
      neCO2 = dataArr[2];
      nTVOC = dataArr[3];
      bme280Temp = dataArr[4];
      bme280Hum = dataArr[5];
      bme280Pres = dataArr[6];
         
      save_data(hdc1080Temp, hdc1080Hum, neCO2, nTVOC, bme280Temp, bme280Hum, bme280Pres);
      
      str = "Success";
      SpreadsheetApp.flush();
      break;
      
  
     case "appendRow2":
      var dataArr = parsedData.values.split(",");
      
      tempIN = dataArr[0];
      humidity = dataArr[1];
      tempOUT = dataArr[2];
      
      save_data2(tempIN, humidity, tempOUT);
      
      str = "Success";
      SpreadsheetApp.flush();
      break;
  }    
    return ContentService.createTextOutput(str);
  }
  else{
    return ContentService.createTextOutput("Error! Request body empty or in incorrect format.");
  }
}

// Method to save given data to a sheet
function save_data(hdc1080Temp, hdc1080Hum, neCO2, nTVOC, bme280Temp, bme280Hum, bme280Pres){
  Logger.log("--- save_data ---"); 
 
 
  try {
    
    var timeZone = Session.getScriptTimeZone();
    var dateTime = Utilities.formatDate(new Date(), timeZone, "MM/dd/yyyy HH:mm:ss");
 
    // Paste the URL of the Google Sheets starting from https thru /edit
    // For e.g.: https://docs.google.com/..../edit 
    var ss = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/plapla234dtrgrgYourTagHEre/edit");
    var dataLoggerSheet = ss.getSheetByName("Data");
    var latestSheet = ss.getSheetByName("latest");
 
 
    // Get last edited row from DataLogger sheet
    var row = dataLoggerSheet.getLastRow() + 1;
 
 
    // Start Populating the data
    dataLoggerSheet.getRange("A" + row).setValue(row -1); // ID
    dataLoggerSheet.getRange("B" + row).setValue(dateTime); // dateTime
    dataLoggerSheet.getRange("C" + row).setValue(hdc1080Temp); 
    dataLoggerSheet.getRange("D" + row).setValue(hdc1080Hum); 
    dataLoggerSheet.getRange("E" + row).setValue(neCO2); 
    dataLoggerSheet.getRange("F" + row).setValue(nTVOC); 
    dataLoggerSheet.getRange("G" + row).setValue(bme280Temp); 
    dataLoggerSheet.getRange("H" + row).setValue(bme280Hum);
    dataLoggerSheet.getRange("I" + row).setValue(bme280Pres);
    
    latestSheet.getRange("A" + 2).setValue(dateTime); // dateTime
    latestSheet.getRange("B" + 2).setValue(hdc1080Temp); 
    latestSheet.getRange("C" + 2).setValue(hdc1080Hum); 
    latestSheet.getRange("D" + 2).setValue(neCO2); 
    latestSheet.getRange("E" + 2).setValue(nTVOC);
    latestSheet.getRange("F" + 2).setValue(bme280Temp); 
    latestSheet.getRange("G" + 2).setValue(bme280Hum);
    latestSheet.getRange("H" + 2).setValue(bme280Pres);
    
    }
 
  catch(error) {
    Logger.log(JSON.stringify(error));
  }
 
  Logger.log("--- save_data end---"); 
}
  function save_data2(tempIN, humidity, tempOUT){
  Logger.log("--- save_data2 ---"); 
 
 
  try {
    
    var timeZone = Session.getScriptTimeZone();
    var dateTime = Utilities.formatDate(new Date(), timeZone, "MM/dd/yyyy HH:mm:ss");
 
    // Paste the URL of the Google Sheets starting from https thru /edit
    // For e.g.: https://docs.google.com/..../edit 
    var ss = SpreadsheetApp.openByUrl("https://docs.google.com/spreadsheets/d/plapla234dtrgrgYourTagHEre/edit");
    var dataLoggerSheet = ss.getSheetByName("Data2");
    var latestSheet = ss.getSheetByName("latest");
 
 
    // Get last edited row from DataLogger sheet
    var row = dataLoggerSheet.getLastRow() + 1;
 
 
    // Start Populating the data
    dataLoggerSheet.getRange("A" + row).setValue(row -1); // ID
    dataLoggerSheet.getRange("B" + row).setValue(dateTime); // dateTime
    dataLoggerSheet.getRange("C" + row).setValue(tempIN); 
    dataLoggerSheet.getRange("D" + row).setValue(humidity); 
    dataLoggerSheet.getRange("E" + row).setValue(tempOUT);
    
    latestSheet.getRange("A" + 5).setValue(dateTime); // dateTime
    latestSheet.getRange("B" + 5).setValue(tempIN); 
    latestSheet.getRange("C" + 5).setValue(humidity); 
    latestSheet.getRange("D" + 5).setValue(tempOUT);
    
    }
 
  catch(error) {
    Logger.log(JSON.stringify(error));
  }
 
  Logger.log("--- save_data2 end---"); 
}