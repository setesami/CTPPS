import root;
import pad_layout;

//----------------------------------------------------------------------------------------------------

xSizeDef = 10cm;
ySizeDef = 8cm;

//----------------------------------------------------------------------------------------------------

string model_tags[];
pen model_pens[];

void AddModel(string tag, pen p)
{
	model_tags.push(tag);
	model_pens.push(p);
}


AddModel("block [06]", heavygreen);
AddModel("bourrely [03]", blue);
AddModel("islam (hp) [06,09]", black);
AddModel("islam (lxg) [06,09]", black+dashed);
AddModel("jenkovszky [11]", magenta);
AddModel("petrov (2p) [02]", red+dashed);
AddModel("petrov (3p) [02]", red);

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

string html_file = "";
string dist_dir = "";

file f_out;

//----------------------------------------------------------------------------------------------------

void StartJob(string hf, string dd)
{
	html_file = hf;
	dist_dir = dd;
	f_out = output(hf);

	write(f_out, "<html>", endl);
	write(f_out, "	<head>", endl);
	write(f_out, "		<link rel=\"stylesheet\" type=\"text/css\" href=\"/elegent.css\">", endl);
	write(f_out, "	</head>", endl);
	write(f_out, "<body>", endl);
}

//----------------------------------------------------------------------------------------------------

void EndJob()
{
	write(f_out, "</body>", endl);
	write(f_out, "</html>", endl);
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void DrawOptimized(real jump_tol, rObject obj, pen p, string label = "")
{
	int N = obj.iExec("GetN");
	guide g;

	real prev_y = 0;
	bool firstPoint = true;
	for (int i = 0; i < N; ++i)
	{
		real[] xa = {0};
		real[] ya = {0};
		obj.vExec("GetPoint", i, xa, ya);

		real x = xa[0];
		real y = ya[0];

		if ((currentpicture.scale.x.scale.logarithmic && x <= 0))
			continue;

		if (x > TGraph_highLimit)
			break;

		real de_y = y - prev_y;

		if (firstPoint || abs(de_y) < jump_tol)
		{
			g = g--Scale((x, y));
		} else {
			draw(g, p, label);
			label = "";	// not to multiply labels in legend
			g = Scale((x, y));
		}

		firstPoint = false;
		prev_y = y;
	}
	
	draw(g, p, label);
}

//----------------------------------------------------------------------------------------------------

frame legend_frame;

void PlotAllModels(string input_file, string template, string legend_title,
	bool optimize=false, real opt_jump=0)
{
	for (int mi : model_tags.keys)
	{
		string obj_path = replace(template, "<model>", model_tags[mi]);
		rObject obj = rGetObj(input_file, obj_path, search=false, error=false);
		if (!obj.valid)
			continue;

		string label = obj.sExec("GetTitle");

		if (optimize)
			DrawOptimized(opt_jump, obj, model_pens[mi], label);
		else
			draw(obj, "l", model_pens[mi], label);
	}

	legend_frame = BuildLegend(legend_title);
}

//----------------------------------------------------------------------------------------------------

void FinalizeFile(string name)
{
	// add grid
	for (int pi : pad_collection.keys)
	{
		SetPad(pad_collection[pi]);

		currentpad.xTicks = NoTicks();
		xaxis(BottomTop, LeftTicks(format="%", extend=true, pTick=black+dotted, ptick=invisible));
		xaxis(Label(currentpad.xLabel, 1), BottomTop, LeftTicks());
		currentpad.xLabel = "";
	
		currentpad.yTicks = NoTicks();
		yaxis(LeftRight, RightTicks(format="%", extend=true, pTick=black+dotted, ptick=invisible));
		yaxis(Label(currentpad.yLabel, 1), LeftRight, RightTicks());
		currentpad.yLabel = "";
	}

	NewPad(false);
	add(legend_frame);
	GShipout(name);
}
//----------------------------------------------------------------------------------------------------

typedef void PlotFcn(string dist_file, string option);

struct PlotDesc
{
	PlotFcn plotFcn;
	string fileName, htmlLabel;
	bool doubleScale;
};

//----------------------------------------------------------------------------------------------------

PlotDesc plots[];

void RegisterPlot(string fn, string hl, bool doubleScale, PlotFcn pf)
{
	PlotDesc pd;
	pd.plotFcn = pf;
	pd.fileName = fn;
	pd.htmlLabel = hl;
	pd.doubleScale = doubleScale;

	plots.push(pd);
}

//----------------------------------------------------------------------------------------------------

void ProcessOnePlot(int pi, string mode, string energies[], string tag, string file_suffix, string obj_prefix, int html_flag)
{
	write(f_out, "	<tr>", endl);

	if (html_flag == 1)
		write(f_out, "		<td class=\"quantity\">"+plots[pi].htmlLabel+"</td>", endl);
	if (html_flag == 2)
		write(f_out, "		<td rowspan=\"2\" class=\"quantity\">"+plots[pi].htmlLabel+"</td>", endl);

	for (int ei : energies.keys)
	{
		string dist_file = dist_dir + "/" + tag + "," + mode + "," + energies[ei] + "GeV.root";

		string out_file = mode+","+energies[ei]+"GeV/"+tag+","+plots[pi].fileName;
		if (file_suffix != "")
			out_file += "," + file_suffix;

		write(">> " + out_file);

		plots[pi].plotFcn(dist_file, obj_prefix);
		FinalizeFile(out_file);

		string link_name = obj_prefix;
		if (link_name == "")
			link_name = "full range";

		write(f_out, "		<td><a href=\""+out_file+".pdf\">"+link_name+"</td>", endl);
	}
	
	write(f_out, "	</tr>", endl);
}

//----------------------------------------------------------------------------------------------------

void ProcessPlots(string mode, string energies[], string tag)
{
	string html_mode = mode;
	if (html_mode == "app")
		html_mode = "p&#773;p";
	write(f_out, "<h2>"+html_mode+"</h2>", endl);
	
	write(f_out, "<table class=\"plot\">", endl);
	write(f_out, "	<tr>", endl);
	write(f_out, "		<td></td><td colspan=\""+format("%u", energies.length)+"\">&radic;s   (GeV<sup>2</sup>)</td>", endl);
	write(f_out, "	</tr>", endl);
	
	write(f_out, "	<tr>", endl);
	write(f_out, "		<td></td>", endl);
	for (int ei : energies.keys)
	{
		write(f_out, "		<td>"+energies[ei]+"</td>", endl);
	}
	write(f_out, "	</tr>", endl);

	for (int pi : plots.keys)
	{
		if (plots[pi].doubleScale)
		{
			TGraph_highLimit = 10;
			ProcessOnePlot(pi, mode, energies, tag, "full_range", "full range", 2);
			TGraph_highLimit = +inf;
			ProcessOnePlot(pi, mode, energies, tag, "low_t", "low |t|", 0);
		} else {
			ProcessOnePlot(pi, mode, energies, tag, "", "", 1);
		}
	}
	
	write(f_out, "</table>", endl);
}

//----------------------------------------------------------------------------------------------------

void ProcessOneSPlot(int pi, string mode, string tag)
{
	write(f_out, "	<tr>", endl);
	write(f_out, "		<td class=\"quantity\">"+plots[pi].htmlLabel+"</td>", endl);

	string dist_file = dist_dir + "/" + tag + "," + mode + ".root";
	string out_file = mode+","+tag+","+plots[pi].fileName;

	write(">> " + out_file);

	plots[pi].plotFcn(dist_file, "");
	FinalizeFile(out_file);

	write(f_out, "		<td><a href=\""+out_file+".pdf\">full range</td>", endl);
	
	write(f_out, "	</tr>", endl);
}

//----------------------------------------------------------------------------------------------------

void ProcessSPlots(string mode, string tag)
{
	string html_mode = mode;
	if (html_mode == "app")
		html_mode = "p&#773;p";
	write(f_out, "<h2>"+html_mode+"</h2>", endl);
	
	write(f_out, "<table class=\"plot\">", endl);
	for (int pi : plots.keys)
	{
		ProcessOneSPlot(pi, mode, tag);
	}
	
	write(f_out, "</table>", endl);
}

//----------------------------------------------------------------------------------------------------

void MakeTPlots(string mode, string energies[])
{
	plots.delete();

	RegisterPlot("amp", "hadronic amplitude", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$\Re F^{\rm H}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, option+"/<model>/PH/amplitude_re", "hadronic amplitudes");
	
		NewPad("$|t|\ung{GeV^2}$", "$\Im F^{\rm H}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, option+"/<model>/PH/amplitude_im", "hadronic amplitudes");
	});
	
	RegisterPlot("diff_cs", "hadronic differential cross-section", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$\d\si / \d t \ung{mb/GeV^2}$");
		scale((option == "low |t|") ? Log : Linear, Log(true));
		PlotAllModels(input_file, "full range/<model>/PH/differential cross-section", "hadronic differential cross-section");
	});
	
	RegisterPlot("B", "hadronic slope", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$B(t) \equiv {\d\over \d t} \log{\d\si\over\d t} \ung{GeV^{-2}}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, "full range/<model>/PH/B", "hadronic slope");
	});
	
	RegisterPlot("phase", "hadronic phase", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$\arg F^{\rm H}(t)$");
		scale((option == "low |t|") ? Log : Linear, Linear);
		PlotAllModels(input_file, "full range/<model>/PH/phase", "hadronic phase", true, 1);
		ylimits(-3.141593, +3.141593, Crop);
	});
	
	RegisterPlot("rho", "hadronic rho", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$\rh(t) \equiv {\Re F^{\rm H}\over \Im F^{\rm H}}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, "full range/<model>/PH/rho", "rho parameter", true, 8);
		ylimits(-10, 10, Crop);
	});
	
	RegisterPlot("C", "influence of Coulomb interaction", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$C(t) \equiv {|F^{\rm C+H}|^2 - |F^{\rm H}|^2 \over |F^{\rm H}|^2}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, "full range/<model>/C", "influence of Coulomb interaction");
		//ylimits(-10, 10, Crop);
	});
	
	RegisterPlot("Z", "importance of the interference term", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$Z(t) \equiv {|F^{\rm C+H}|^2 - |F^{\rm H}|^2 - |F^{\rm C}|^2 \over |F^{\rm C+H}|^2}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, "full range/<model>/Z", "importance of the interference term");
		//ylimits(-10, 10, Crop);
	});
	
	RegisterPlot("R", "difference between SWY and KL formulae", doubleScale=true, new void (string input_file, string option) {
		NewPad("$|t|\ung{GeV^2}$", "$R(t) \equiv {|F^{\rm KL}|^2 - |F^{\rm WY}|^2 \over |F^{\rm KL}|^2}$");
		scale((option == "low |t|") ? Log : Linear, Linear(true));
		PlotAllModels(input_file, "full range/<model>/R", "difference between SWY and KL formulae");
		//ylimits(-10, 10, Crop);
	});

	ProcessPlots(mode, energies, "t-distributions");
}

//----------------------------------------------------------------------------------------------------

void MakeBPlots(string mode, string energies[])
{
	plots.delete();

	RegisterPlot("amp", "hadronic amplitude", doubleScale=false, new void (string input_file, string option) {
		NewPad("$b\ung{fm}$", "$\Re A^{\rm H}$");
		scale(Linear, Linear(true));
		PlotAllModels(input_file, "<model>/prf_re", "hadronic amplitudes");
	
		NewPad("$b\ung{fm}$", "$\Im A^{\rm H}$");
		scale(Linear, Linear(true));
		PlotAllModels(input_file, "<model>/prf_im", "hadronic amplitudes");
	});
	
	ProcessPlots(mode, energies, "b-distributions");
}

//----------------------------------------------------------------------------------------------------

void MakeSPlots(string mode)
{

	plots.delete();

	RegisterPlot("si_tot", "hadronic total cross-section", doubleScale=true, new void (string input_file, string option) {
		NewPad("$\sqrt s\ung{GeV}$", "$\si_{\rm tot}(s)\ung{mb}$");
		scale(Log, Linear(true));
		PlotAllModels(input_file, "<model>/si_tot", "hadronic total cross-section");
	});

	RegisterPlot("rho", "hadronic forward rho", doubleScale=true, new void (string input_file, string option) {
		NewPad("$\sqrt s\ung{GeV}$", "$\rh(s, t = 0) \equiv \left. {\Re F^{\rm H}\over \Im F^{\rm H}} \right|_{t = 0}$");
		scale(Log, Linear(true));
		PlotAllModels(input_file, "<model>/rho", "hadronic forward rho");
	});
	
	RegisterPlot("B0", "hadronic forward slope", doubleScale=true, new void (string input_file, string option) {
		NewPad("$\sqrt s\ung{GeV}$", "$B(s, t=0) \equiv \left . {\d \log |F^{\rm H}|^2\over\d t} \right|_{t = 0}\ung{GeV^{-2}}$");
		scale(Log, Linear(true));
		PlotAllModels(input_file, "<model>/B0", "hadronic forward slope");
	});
	
	ProcessSPlots(mode, "s-distributions");
}

//----------------------------------------------------------------------------------------------------

void MakePlots(string pp_energies_str, string app_energies_str)
{
	string pp_energies_a[] = split(pp_energies_str, " ");
	string app_energies_a[] = split(app_energies_str, " ");

	write(f_out, "<h1>t-distributions</h1>", endl);
	MakeTPlots("pp", pp_energies_a);
	MakeTPlots("app", app_energies_a);

	write(f_out, "<h1>b-distributions</h1>", endl);
	MakeBPlots("pp", pp_energies_a);
	MakeBPlots("app", app_energies_a);
	
	write(f_out, "<h1>s-distributions</h1>", endl);
	MakeSPlots("pp");
	MakeSPlots("app");
}
