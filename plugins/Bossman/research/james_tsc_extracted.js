
    // create links to all the tone stack pages to the top of the page
    createLinks("linkDiv", "James", "Passive / Dual Bass Capacitor");

    // component variables for calculation
    var RTreble;
    var RBass;
    var RIN;
    var RL;
    var R1;
    var R2;
    var R3;
    var CB1;
    var CB2;
    var CT1;
    var CT2;

    // create sliders and save the div-element of the slider
    var range_RB = createSlider('range_RB','value_RB');
    var range_RT = createSlider('range_RT','value_RT');

    // Graph objects
    var X = createFrequencies(90,10,100000,1);
    var graph1 = new tscGraph(X, "frequency [Hz]", "amplitude [dB]", "James");
    var graph2 = new tscGraph(X, "frequency [Hz]", "phase", "James", 0, "graph2");
    currentGraph = graph1;

    initializeForm();

    // Set default part values, plus any customizations
    setDefaultValues();
    setCustomValues();

    // Apply part values and display settings
    applyValues();
    if (document.frm.graphToggle.checked) {
        swapGraphs();
    }


    // this function is called when the Apply-button is clicked
    function applyValues() {
        const f = document.frm;

        RIN     = tscResistance.parseElement(f.RIN);
        R1      = tscResistance.parseElement(f.R1);
        RBass   = tscResistance.parseElement(f.RB);
        R2      = tscResistance.parseElement(f.R3);
        R3      = tscResistance.parseElement(f.R4);
        RTreble = tscResistance.parseElement(f.RT);
        RL      = tscResistance.parseElement(f.RL);
        CB1     = tscCapacitance.parseElement(f.C1);
        CB2     = tscCapacitance.parseElement(f.C2);
        CT1     = tscCapacitance.parseElement(f.C3);
        CT2     = tscCapacitance.parseElement(f.C4);

        // set the input fields to readonly mode
        var inputElements = f.getElementsByTagName('input');
        for (var i in inputElements) {
            if (inputElements[i].type == "text") {
                inputElements[i].setAttribute('readonly', 'readonly');
            }
        }

        updateURL();
        doCalc();
    }


    // this function is called when the Edit-button is clicked
    function editValues() {
        var inputElements = document.frm.getElementsByTagName('input');
        for (var i in inputElements) {
            if (inputElements[i].type == "text") {
                inputElements[i].removeAttribute('readonly');
            }
        }
    }


    // This function resets form fields to their default values
    function setDefaultValues() {
        const f = document.frm;

        // Pair form element names to default values
        const defaultValues = {
            RIN    : "38k",
            R1     : "100k",
            RB     : "1M",
            R3     : "10k",
            R4     : "180k",
            RT     : "470k",
            RL     : "1M",
            C1     : "470p",
            C2     : "4700p",
            C3     : "330p",
            C4     : "3300p",
            RB_pot : "LogB",
            RT_pot : "LogB",
        }

        // Set default values and clear any custom form validation errors
        for (var key in defaultValues) {
            f[key].value = defaultValues[key];
            f[key].setCustomValidity("");
        }
    }


    // This function creates the sweep for extreme values
    function sweepValues() {
        for (p = 0; p <= 10; p += 5) {
            range_RB.noUiSlider.set(p);
            for (q = 0; q <=10; q += 5) {
                range_RT.noUiSlider.set(q);
                addSeries();
            }
        }
    }


    // This function is called with every change of the slider.
    // It does the hard math and evaluates the filter transfer function
    // for the given frequency range and updates values to the graph.
    function doCalc() {

        // pot rotation 0 - 10 from the slider component
        var RotTreble = getRotationForPotType(range_RT.noUiSlider.get(), document.frm.RT_pot.value);
        var RotBass   = getRotationForPotType(range_RB.noUiSlider.get(), document.frm.RB_pot.value);

        // Calculated part values
        var RB2  = Math.round( RBass*(1.0*RotBass/10.0) );
        var RB1  = Math.round( RBass*(1.0 - 1.0*RotBass/10.0) );
        var RT2  = Math.round( RTreble*(1.0*RotTreble/10.0) );
        var RT1  = Math.round( RTreble*(1.0 - 1.0*RotTreble/10.0) );


        // Transfer function denominator coefficients

var DEN_XRe = CB1*CB2*CT1*CT2*RB1*RB2*RIN*RL*RT1*RT2
 + CB1*CB2*CT1*CT2*R2*RB1*RB2*RL*RT1*RT2
 + CB1*CB2*CT1*CT2*R1*RB1*RB2*RL*RT1*RT2
 + CB1*CB2*CT1*CT2*R3*RB1*RB2*RIN*RT1*RT2
 + CB1*CB2*CT1*CT2*R2*RB1*RB2*RIN*RT1*RT2
 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RT1*RT2
 + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RT1*RT2
 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RT1*RT2
 + CB1*CB2*CT1*CT2*R3*RB1*RB2*RIN*RL*RT2
 + CB1*CB2*CT1*CT2*R1*RB1*RB2*RIN*RL*RT2 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RL*RT2
 + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RL*RT2 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RL*RT2
 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RIN*RT2
 + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RIN*RT2
 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RIN*RT2
 + CB1*CB2*CT1*CT2*R3*RB1*RB2*RIN*RL*RT1
 + CB1*CB2*CT1*CT2*R2*RB1*RB2*RIN*RL*RT1 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RL*RT1
 + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RL*RT1 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RL*RT1
 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RIN*RL + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RIN*RL
 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RIN*RL;

var DEN_AIm = CB2*CT1*CT2*RB2*RIN*RL*RT1*RT2 + CB1*CT1*CT2*RB1*RIN*RL*RT1*RT2
 + CB2*CT1*CT2*RB1*RB2*RL*RT1*RT2 + CB1*CT1*CT2*RB1*RB2*RL*RT1*RT2
 + CB2*CT1*CT2*R2*RB2*RL*RT1*RT2 + CB2*CT1*CT2*R1*RB2*RL*RT1*RT2
 + CB1*CT1*CT2*R2*RB1*RL*RT1*RT2 + CB1*CT1*CT2*R1*RB1*RL*RT1*RT2
 + CB1*CT1*CT2*RB1*RB2*RIN*RT1*RT2 + CB2*CT1*CT2*R3*RB2*RIN*RT1*RT2
 + CB2*CT1*CT2*R2*RB2*RIN*RT1*RT2 + CB1*CT1*CT2*R3*RB1*RIN*RT1*RT2
 + CB1*CT1*CT2*R2*RB1*RIN*RT1*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RT1*RT2
 + CB1*CT1*CT2*R3*RB1*RB2*RT1*RT2 + CB2*CT1*CT2*R2*RB1*RB2*RT1*RT2
 + CB1*CT1*CT2*R1*RB1*RB2*RT1*RT2 + CB2*CT1*CT2*R2*R3*RB2*RT1*RT2
 + CB2*CT1*CT2*R1*R3*RB2*RT1*RT2 + CB2*CT1*CT2*R1*R2*RB2*RT1*RT2
 + CB1*CT1*CT2*R2*R3*RB1*RT1*RT2 + CB1*CT1*CT2*R1*R3*RB1*RT1*RT2
 + CB1*CT1*CT2*R1*R2*RB1*RT1*RT2 + CB2*CT1*CT2*RB1*RB2*RIN*RL*RT2
 + CB1*CB2*CT2*RB1*RB2*RIN*RL*RT2 + CB2*CT1*CT2*R3*RB2*RIN*RL*RT2
 + CB2*CT1*CT2*R1*RB2*RIN*RL*RT2 + CB1*CT1*CT2*R3*RB1*RIN*RL*RT2
 + CB1*CT1*CT2*R1*RB1*RIN*RL*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RL*RT2
 + CB1*CT1*CT2*R3*RB1*RB2*RL*RT2 + CB2*CT1*CT2*R2*RB1*RB2*RL*RT2
 + CB1*CB2*CT2*R2*RB1*RB2*RL*RT2 + CB1*CT1*CT2*R1*RB1*RB2*RL*RT2
 + CB1*CB2*CT2*R1*RB1*RB2*RL*RT2 + CB2*CT1*CT2*R2*R3*RB2*RL*RT2
 + CB2*CT1*CT2*R1*R3*RB2*RL*RT2 + CB2*CT1*CT2*R1*R2*RB2*RL*RT2
 + CB1*CT1*CT2*R2*R3*RB1*RL*RT2 + CB1*CT1*CT2*R1*R3*RB1*RL*RT2
 + CB1*CT1*CT2*R1*R2*RB1*RL*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RIN*RT2
 + CB1*CT1*CT2*R3*RB1*RB2*RIN*RT2 + CB1*CB2*CT2*R3*RB1*RB2*RIN*RT2
 + CB2*CT1*CT2*R2*RB1*RB2*RIN*RT2 + CB1*CB2*CT2*R2*RB1*RB2*RIN*RT2
 + CB1*CT1*CT2*R1*RB1*RB2*RIN*RT2 + CB2*CT1*CT2*R2*R3*RB2*RIN*RT2
 + CB2*CT1*CT2*R1*R3*RB2*RIN*RT2 + CB2*CT1*CT2*R1*R2*RB2*RIN*RT2
 + CB1*CT1*CT2*R2*R3*RB1*RIN*RT2 + CB1*CT1*CT2*R1*R3*RB1*RIN*RT2
 + CB1*CT1*CT2*R1*R2*RB1*RIN*RT2 + CB1*CB2*CT2*R2*R3*RB1*RB2*RT2
 + CB1*CB2*CT2*R1*R3*RB1*RB2*RT2 + CB1*CB2*CT2*R1*R2*RB1*RB2*RT2
 + CB1*CT1*CT2*RB1*RB2*RIN*RL*RT1 + CB1*CB2*CT1*RB1*RB2*RIN*RL*RT1
 + CB2*CT1*CT2*R3*RB2*RIN*RL*RT1 + CB2*CT1*CT2*R2*RB2*RIN*RL*RT1
 + CB1*CT1*CT2*R3*RB1*RIN*RL*RT1 + CB1*CT1*CT2*R2*RB1*RIN*RL*RT1
 + CB2*CT1*CT2*R3*RB1*RB2*RL*RT1 + CB1*CT1*CT2*R3*RB1*RB2*RL*RT1
 + CB2*CT1*CT2*R2*RB1*RB2*RL*RT1 + CB1*CB2*CT1*R2*RB1*RB2*RL*RT1
 + CB1*CT1*CT2*R1*RB1*RB2*RL*RT1 + CB1*CB2*CT1*R1*RB1*RB2*RL*RT1
 + CB2*CT1*CT2*R2*R3*RB2*RL*RT1 + CB2*CT1*CT2*R1*R3*RB2*RL*RT1
 + CB2*CT1*CT2*R1*R2*RB2*RL*RT1 + CB1*CT1*CT2*R2*R3*RB1*RL*RT1
 + CB1*CT1*CT2*R1*R3*RB1*RL*RT1 + CB1*CT1*CT2*R1*R2*RB1*RL*RT1
 + CB1*CB2*CT1*R3*RB1*RB2*RIN*RT1 + CB1*CB2*CT1*R2*RB1*RB2*RIN*RT1
 + CB1*CB2*CT1*R2*R3*RB1*RB2*RT1 + CB1*CB2*CT1*R1*R3*RB1*RB2*RT1
 + CB1*CB2*CT1*R1*R2*RB1*RB2*RT1 + CB2*CT1*CT2*R3*RB1*RB2*RIN*RL
 + CB1*CT1*CT2*R3*RB1*RB2*RIN*RL + CB1*CB2*CT2*R3*RB1*RB2*RIN*RL
 + CB1*CB2*CT1*R3*RB1*RB2*RIN*RL + CB2*CT1*CT2*R2*RB1*RB2*RIN*RL
 + CB1*CB2*CT2*R2*RB1*RB2*RIN*RL + CB1*CT1*CT2*R1*RB1*RB2*RIN*RL
 + CB1*CB2*CT1*R1*RB1*RB2*RIN*RL + CB2*CT1*CT2*R2*R3*RB2*RIN*RL
 + CB2*CT1*CT2*R1*R3*RB2*RIN*RL + CB2*CT1*CT2*R1*R2*RB2*RIN*RL
 + CB1*CT1*CT2*R2*R3*RB1*RIN*RL + CB1*CT1*CT2*R1*R3*RB1*RIN*RL
 + CB1*CT1*CT2*R1*R2*RB1*RIN*RL + CB1*CB2*CT2*R2*R3*RB1*RB2*RL
 + CB1*CB2*CT1*R2*R3*RB1*RB2*RL + CB1*CB2*CT2*R1*R3*RB1*RB2*RL
 + CB1*CB2*CT1*R1*R3*RB1*RB2*RL + CB1*CB2*CT2*R1*R2*RB1*RB2*RL
 + CB1*CB2*CT1*R1*R2*RB1*RB2*RL + CB1*CB2*CT1*R2*R3*RB1*RB2*RIN
 + CB1*CB2*CT1*R1*R3*RB1*RB2*RIN + CB1*CB2*CT1*R1*R2*RB1*RB2*RIN;

var DEN_BRe = CT1*CT2*RIN*RL*RT1*RT2 + CT1*CT2*RB2*RL*RT1*RT2
 + CT1*CT2*RB1*RL*RT1*RT2 + CT1*CT2*R2*RL*RT1*RT2 + CT1*CT2*R1*RL*RT1*RT2
 + CT1*CT2*RB2*RIN*RT1*RT2 + CT1*CT2*R3*RIN*RT1*RT2 + CT1*CT2*R2*RIN*RT1*RT2
 + CT1*CT2*RB1*RB2*RT1*RT2 + CT1*CT2*R3*RB2*RT1*RT2 + CT1*CT2*R1*RB2*RT1*RT2
 + CT1*CT2*R3*RB1*RT1*RT2 + CT1*CT2*R2*RB1*RT1*RT2 + CT1*CT2*R2*R3*RT1*RT2
 + CT1*CT2*R1*R3*RT1*RT2 + CT1*CT2*R1*R2*RT1*RT2 + CB2*CT2*RB2*RIN*RL*RT2
 + CT1*CT2*RB1*RIN*RL*RT2 + CB1*CT2*RB1*RIN*RL*RT2 + CT1*CT2*R3*RIN*RL*RT2
 + CT1*CT2*R1*RIN*RL*RT2 + CT1*CT2*RB1*RB2*RL*RT2 + CB2*CT2*RB1*RB2*RL*RT2
 + CB1*CT2*RB1*RB2*RL*RT2 + CT1*CT2*R3*RB2*RL*RT2 + CB2*CT2*R2*RB2*RL*RT2
 + CT1*CT2*R1*RB2*RL*RT2 + CB2*CT2*R1*RB2*RL*RT2 + CT1*CT2*R3*RB1*RL*RT2
 + CT1*CT2*R2*RB1*RL*RT2 + CB1*CT2*R2*RB1*RL*RT2 + CB1*CT2*R1*RB1*RL*RT2
 + CT1*CT2*R2*R3*RL*RT2 + CT1*CT2*R1*R3*RL*RT2 + CT1*CT2*R1*R2*RL*RT2
 + CT1*CT2*RB1*RB2*RIN*RT2 + CB1*CT2*RB1*RB2*RIN*RT2 + CT1*CT2*R3*RB2*RIN*RT2
 + CB2*CT2*R3*RB2*RIN*RT2 + CB2*CT2*R2*RB2*RIN*RT2 + CT1*CT2*R1*RB2*RIN*RT2
 + CT1*CT2*R3*RB1*RIN*RT2 + CB1*CT2*R3*RB1*RIN*RT2 + CT1*CT2*R2*RB1*RIN*RT2
 + CB1*CT2*R2*RB1*RIN*RT2 + CT1*CT2*R2*R3*RIN*RT2 + CT1*CT2*R1*R3*RIN*RT2
 + CT1*CT2*R1*R2*RIN*RT2 + CB2*CT2*R3*RB1*RB2*RT2 + CB1*CT2*R3*RB1*RB2*RT2
 + CB2*CT2*R2*RB1*RB2*RT2 + CB1*CT2*R1*RB1*RB2*RT2 + CB2*CT2*R2*R3*RB2*RT2
 + CB2*CT2*R1*R3*RB2*RT2 + CB2*CT2*R1*R2*RB2*RT2 + CB1*CT2*R2*R3*RB1*RT2
 + CB1*CT2*R1*R3*RB1*RT2 + CB1*CT2*R1*R2*RB1*RT2 + CT1*CT2*RB2*RIN*RL*RT1
 + CB2*CT1*RB2*RIN*RL*RT1 + CB1*CT1*RB1*RIN*RL*RT1 + CT1*CT2*R3*RIN*RL*RT1
 + CT1*CT2*R2*RIN*RL*RT1 + CT1*CT2*RB1*RB2*RL*RT1 + CB2*CT1*RB1*RB2*RL*RT1
 + CB1*CT1*RB1*RB2*RL*RT1 + CT1*CT2*R3*RB2*RL*RT1 + CB2*CT1*R2*RB2*RL*RT1
 + CT1*CT2*R1*RB2*RL*RT1 + CB2*CT1*R1*RB2*RL*RT1 + CT1*CT2*R3*RB1*RL*RT1
 + CT1*CT2*R2*RB1*RL*RT1 + CB1*CT1*R2*RB1*RL*RT1 + CB1*CT1*R1*RB1*RL*RT1
 + CT1*CT2*R2*R3*RL*RT1 + CT1*CT2*R1*R3*RL*RT1 + CT1*CT2*R1*R2*RL*RT1
 + CB1*CT1*RB1*RB2*RIN*RT1 + CB2*CT1*R3*RB2*RIN*RT1 + CB2*CT1*R2*RB2*RIN*RT1
 + CB1*CT1*R3*RB1*RIN*RT1 + CB1*CT1*R2*RB1*RIN*RT1 + CB2*CT1*R3*RB1*RB2*RT1
 + CB1*CT1*R3*RB1*RB2*RT1 + CB2*CT1*R2*RB1*RB2*RT1 + CB1*CT1*R1*RB1*RB2*RT1
 + CB2*CT1*R2*R3*RB2*RT1 + CB2*CT1*R1*R3*RB2*RT1 + CB2*CT1*R1*R2*RB2*RT1
 + CB1*CT1*R2*R3*RB1*RT1 + CB1*CT1*R1*R3*RB1*RT1 + CB1*CT1*R1*R2*RB1*RT1
 + CT1*CT2*RB1*RB2*RIN*RL + CB1*CT2*RB1*RB2*RIN*RL + CB2*CT1*RB1*RB2*RIN*RL
 + CB1*CB2*RB1*RB2*RIN*RL + CT1*CT2*R3*RB2*RIN*RL + CB2*CT2*R3*RB2*RIN*RL
 + CB2*CT1*R3*RB2*RIN*RL + CB2*CT2*R2*RB2*RIN*RL + CT1*CT2*R1*RB2*RIN*RL
 + CB2*CT1*R1*RB2*RIN*RL + CT1*CT2*R3*RB1*RIN*RL + CB1*CT2*R3*RB1*RIN*RL
 + CB1*CT1*R3*RB1*RIN*RL + CT1*CT2*R2*RB1*RIN*RL + CB1*CT2*R2*RB1*RIN*RL
 + CB1*CT1*R1*RB1*RIN*RL + CT1*CT2*R2*R3*RIN*RL + CT1*CT2*R1*R3*RIN*RL
 + CT1*CT2*R1*R2*RIN*RL + CB2*CT2*R3*RB1*RB2*RL + CB1*CT2*R3*RB1*RB2*RL
 + CB2*CT1*R3*RB1*RB2*RL + CB1*CT1*R3*RB1*RB2*RL + CB2*CT2*R2*RB1*RB2*RL
 + CB2*CT1*R2*RB1*RB2*RL + CB1*CB2*R2*RB1*RB2*RL + CB1*CT2*R1*RB1*RB2*RL
 + CB1*CT1*R1*RB1*RB2*RL + CB1*CB2*R1*RB1*RB2*RL + CB2*CT2*R2*R3*RB2*RL
 + CB2*CT1*R2*R3*RB2*RL + CB2*CT2*R1*R3*RB2*RL + CB2*CT1*R1*R3*RB2*RL
 + CB2*CT2*R1*R2*RB2*RL + CB2*CT1*R1*R2*RB2*RL + CB1*CT2*R2*R3*RB1*RL
 + CB1*CT1*R2*R3*RB1*RL + CB1*CT2*R1*R3*RB1*RL + CB1*CT1*R1*R3*RB1*RL
 + CB1*CT2*R1*R2*RB1*RL + CB1*CT1*R1*R2*RB1*RL + CB2*CT1*R3*RB1*RB2*RIN
 + CB1*CT1*R3*RB1*RB2*RIN + CB1*CB2*R3*RB1*RB2*RIN + CB2*CT1*R2*RB1*RB2*RIN
 + CB1*CB2*R2*RB1*RB2*RIN + CB1*CT1*R1*RB1*RB2*RIN + CB2*CT1*R2*R3*RB2*RIN
 + CB2*CT1*R1*R3*RB2*RIN + CB2*CT1*R1*R2*RB2*RIN + CB1*CT1*R2*R3*RB1*RIN
 + CB1*CT1*R1*R3*RB1*RIN + CB1*CT1*R1*R2*RB1*RIN + CB1*CB2*R2*R3*RB1*RB2
 + CB1*CB2*R1*R3*RB1*RB2 + CB1*CB2*R1*R2*RB1*RB2;

var DEN_CIm = CT2*RIN*RL*RT2 + CT2*RB2*RL*RT2 + CT2*RB1*RL*RT2 + CT2*R2*RL*RT2
 + CT2*R1*RL*RT2 + CT2*RB2*RIN*RT2 + CT2*R3*RIN*RT2 + CT2*R2*RIN*RT2
 + CT2*RB1*RB2*RT2 + CT2*R3*RB2*RT2 + CT2*R1*RB2*RT2 + CT2*R3*RB1*RT2
 + CT2*R2*RB1*RT2 + CT2*R2*R3*RT2 + CT2*R1*R3*RT2 + CT2*R1*R2*RT2
 + CT1*RIN*RL*RT1 + CT1*RB2*RL*RT1 + CT1*RB1*RL*RT1 + CT1*R2*RL*RT1
 + CT1*R1*RL*RT1 + CT1*RB2*RIN*RT1 + CT1*R3*RIN*RT1 + CT1*R2*RIN*RT1
 + CT1*RB1*RB2*RT1 + CT1*R3*RB2*RT1 + CT1*R1*RB2*RT1 + CT1*R3*RB1*RT1
 + CT1*R2*RB1*RT1 + CT1*R2*R3*RT1 + CT1*R1*R3*RT1 + CT1*R1*R2*RT1
 + CT2*RB2*RIN*RL + CB2*RB2*RIN*RL + CT1*RB1*RIN*RL + CB1*RB1*RIN*RL
 + CT2*R3*RIN*RL + CT1*R3*RIN*RL + CT2*R2*RIN*RL + CT1*R1*RIN*RL
 + CT2*RB1*RB2*RL + CT1*RB1*RB2*RL + CB2*RB1*RB2*RL + CB1*RB1*RB2*RL
 + CT2*R3*RB2*RL + CT1*R3*RB2*RL + CB2*R2*RB2*RL + CT2*R1*RB2*RL
 + CT1*R1*RB2*RL + CB2*R1*RB2*RL + CT2*R3*RB1*RL + CT1*R3*RB1*RL
 + CT2*R2*RB1*RL + CT1*R2*RB1*RL + CB1*R2*RB1*RL + CB1*R1*RB1*RL + CT2*R2*R3*RL
 + CT1*R2*R3*RL + CT2*R1*R3*RL + CT1*R1*R3*RL + CT2*R1*R2*RL + CT1*R1*R2*RL
 + CT1*RB1*RB2*RIN + CB1*RB1*RB2*RIN + CT1*R3*RB2*RIN + CB2*R3*RB2*RIN
 + CB2*R2*RB2*RIN + CT1*R1*RB2*RIN + CT1*R3*RB1*RIN + CB1*R3*RB1*RIN
 + CT1*R2*RB1*RIN + CB1*R2*RB1*RIN + CT1*R2*R3*RIN + CT1*R1*R3*RIN
 + CT1*R1*R2*RIN + CB2*R3*RB1*RB2 + CB1*R3*RB1*RB2 + CB2*R2*RB1*RB2
 + CB1*R1*RB1*RB2 + CB2*R2*R3*RB2 + CB2*R1*R3*RB2 + CB2*R1*R2*RB2
 + CB1*R2*R3*RB1 + CB1*R1*R3*RB1 + CB1*R1*R2*RB1;

var DEN_DRe =  RIN*RL + RB2*RL + RB1*RL + R2*RL + R1*RL + RB2*RIN + R3*RIN + R2*RIN
 + RB1*RB2 + R3*RB2 + R1*RB2 + R3*RB1 + R2*RB1 + R2*R3 + R1*R3 + R1*R2;


// Transfer function numerator multipliers for current I5-I6

var NOM_XRe = CB1*CB2*CT1*CT2*R2*RB1*RB2*RL*RT1*RT2
 + CB1*CB2*CT1*CT2*R2*R3*RB1*RB2*RL*RT2 + CB1*CB2*CT1*CT2*R1*R3*RB1*RB2*RL*RT2
 + CB1*CB2*CT1*CT2*R1*R2*RB1*RB2*RL*RT2;

var NOM_AIm = CB1*CT1*CT2*RB1*RB2*RL*RT1*RT2 + CB2*CT1*CT2*R2*RB2*RL*RT1*RT2
 + CB1*CT1*CT2*R2*RB1*RL*RT1*RT2 + CB2*CT1*CT2*R3*RB1*RB2*RL*RT2
 + CB1*CT1*CT2*R3*RB1*RB2*RL*RT2 + CB2*CT1*CT2*R2*RB1*RB2*RL*RT2
 + CB1*CB2*CT2*R2*RB1*RB2*RL*RT2 + CB1*CT1*CT2*R1*RB1*RB2*RL*RT2
 + CB2*CT1*CT2*R2*R3*RB2*RL*RT2 + CB2*CT1*CT2*R1*R3*RB2*RL*RT2
 + CB2*CT1*CT2*R1*R2*RB2*RL*RT2 + CB1*CT1*CT2*R2*R3*RB1*RL*RT2
 + CB1*CT1*CT2*R1*R3*RB1*RL*RT2 + CB1*CT1*CT2*R1*R2*RB1*RL*RT2
 + CB1*CB2*CT1*R2*RB1*RB2*RL*RT1 + CB1*CB2*CT1*R2*R3*RB1*RB2*RL
 + CB1*CB2*CT1*R1*R3*RB1*RB2*RL + CB1*CB2*CT1*R1*R2*RB1*RB2*RL;

var NOM_BRe = CT1*CT2*RB2*RL*RT1*RT2 + CT1*CT2*R2*RL*RT1*RT2 + CT1*CT2*RB1*RB2*RL*RT2
 + CB1*CT2*RB1*RB2*RL*RT2 + CT1*CT2*R3*RB2*RL*RT2 + CB2*CT2*R2*RB2*RL*RT2
 + CT1*CT2*R1*RB2*RL*RT2 + CT1*CT2*R3*RB1*RL*RT2 + CT1*CT2*R2*RB1*RL*RT2
 + CB1*CT2*R2*RB1*RL*RT2 + CT1*CT2*R2*R3*RL*RT2 + CT1*CT2*R1*R3*RL*RT2
 + CT1*CT2*R1*R2*RL*RT2 + CB1*CT1*RB1*RB2*RL*RT1 + CB2*CT1*R2*RB2*RL*RT1
 + CB1*CT1*R2*RB1*RL*RT1 + CB2*CT1*R3*RB1*RB2*RL + CB1*CT1*R3*RB1*RB2*RL
 + CB2*CT1*R2*RB1*RB2*RL + CB1*CB2*R2*RB1*RB2*RL + CB1*CT1*R1*RB1*RB2*RL
 + CB2*CT1*R2*R3*RB2*RL + CB2*CT1*R1*R3*RB2*RL + CB2*CT1*R1*R2*RB2*RL
 + CB1*CT1*R2*R3*RB1*RL + CB1*CT1*R1*R3*RB1*RL + CB1*CT1*R1*R2*RB1*RL;

var NOM_CIm = CT2*RB2*RL*RT2 + CT2*R2*RL*RT2 + CT1*RB2*RL*RT1 + CT1*R2*RL*RT1
 + CT1*RB1*RB2*RL + CB1*RB1*RB2*RL + CT1*R3*RB2*RL + CB2*R2*RB2*RL
 + CT1*R1*RB2*RL + CT1*R3*RB1*RL + CT1*R2*RB1*RL + CB1*R2*RB1*RL + CT1*R2*R3*RL
 + CT1*R1*R3*RL + CT1*R1*R2*RL;

var NOM_DRe = RB2*RL + R2*RL;


        // Calculate magnitude and phase at each frequency
        doCalcBode(
            [ NOM_DRe, NOM_CIm, NOM_BRe, NOM_AIm, NOM_XRe ],
            [ DEN_DRe, DEN_CIm, DEN_BRe, DEN_AIm, DEN_XRe ]
        );
        currentGraph.update();
    }
