/*******************  Set the theme for each graph /*******************/

Highcharts.setOptions({
  chart: { type: 'spline', zoomType: 'xy' },
  xAxis: { 
    gridLineWidth: 1, 
    tickPixelInterval: 50, 
    labels: { format: '{value} s' }
  },
  yAxis: { // minorTickInterval: 'auto',
    gridLineWidth: 1,
  },
  plotOptions: {
    spline: {
       lineWidth: 1,
       states: { hover: { lineWidth: 2 } },
       marker: { height: 5, enabled: true, fillColor: 'orange' },
       minorTickLength: 0.5,
    }
  },
});

/******************* For each category of graphs *******************/

// Custom options for graph 1 and 2 : Window Size
var optionsWindowSize = {
  xAxis: { title: { text: 'Time [s]' } },
  yAxis: { title: { text: 'Window Size [B]' } },
  tooltip: { valueSuffix: ' bytes' },
  series: []
};

// Custom options for graph 3 and 4 : Bandwidth 
var optionsBandwidth = {
  xAxis: { title: { text: 'Time [s]' } },
  yAxis: { title: { text: 'Throuthput [B]' } },
  tooltip: { valueSuffix: ' bits' },
  series: []
};

// Custom options for graph 5 and 6 : RTT
var optionsRTT = {
  xAxis: { title: { text: 'Relative Sequence Number' }, labels: { format: '{value}'} },
  yAxis: { title: { text: 'RTT [ms]' } },
  tooltip: { valueSuffix: ' ms' },
  series: []
};

// Custom options for graph 7 and 8 : Sequential numbers
var optionsSeqNum = {
  xAxis: { title: { text: 'Time [s]' } },
  yAxis: { title: { text: 'Relative sequence number' } },
  series: []
};

/******************* For each graph *******************/

var SendWindow = {
  chart: { renderTo: 'containerSendWnd' },
  title: { text: 'Sender\'s Window Size' },
};

var RecWindow= {
  chart: { renderTo: 'containerRecWnd' },
  title: { text: 'Receiver\'s Window Size' },
};

var SendBandwith = {
  chart: { renderTo: 'containerSendBnd' },
  title: { text: 'Sender\'s Bandwith' },
};

var RecBandwith = {
  chart: { renderTo: 'containerRecBnd' },
  title: { text: 'Receiver\'s Bandwith' },
};

var SendRTT = {
  chart: { renderTo: 'containerSendRTT' },
  title: { text: 'Sender\'s Round Trip Time' },
};

var RecRTT = {
  chart: { renderTo: 'containerRecRTT' },
  title: { text: 'Receiver\'s Round Trip Time' },
};

var SendSeqNum = {
  chart: { renderTo: 'containerSendSeq' },
  title: { text: 'Sender\'s Relative Sequence numbers' },
};

var RecSeqNum = {
  chart: { renderTo: 'containerRecSeq' },
  title: { text: 'Receiver\'s Relative Sequence numbers' },
};


$(document).ready(function() {
    updateChart();
});

function updateChart() {
  $.getJSON('log/tcpLog.json', 
    function(data) {

      /******************* Initialization of graphs *******************/
      chartRecWindowOptions = jQuery.extend(true, {}, optionsWindowSize, SendWindow);
      chartRecWindowOptions.series.push(data["SendWindow"][0]);
      var chart1 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsWindowSize, RecWindow);
      chartRecWindowOptions.series.push(data["RecWindow"][0]);
      var chart2 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsBandwidth, SendBandwith);
      chartRecWindowOptions.series.push(data["SendBandwith"][0]);
      var chart3 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsBandwidth, RecBandwith);
      chartRecWindowOptions.series.push(data["RecBandwith"][0]);
      var chart4 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsRTT, SendRTT);
      chartRecWindowOptions.series.push(data["SendRTT"][0]);
      var chart5 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsRTT, RecRTT);
      chartRecWindowOptions.series.push(data["RecRTT"][0]);
      var chart6 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsSeqNum, SendSeqNum);
      chartRecWindowOptions.series.push(data["SendSeqNum"][0]);
      var chart7 = new Highcharts.Chart(chartRecWindowOptions);

      chartRecWindowOptions = jQuery.extend(true, {}, optionsSeqNum, RecSeqNum);
      chartRecWindowOptions.series.push(data["RecSeqNum"][0]);
      var chart8 = new Highcharts.Chart(chartRecWindowOptions);
  })
};
