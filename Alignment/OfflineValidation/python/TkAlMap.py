import os
import ROOT
import copy
import math
import time
import ctypes
import argparse
from array import array
import FWCore.ParameterSet.Config as cms
from Alignment.OfflineValidation.DivergingColor import DivergingColor

'''
Class for the TkAlMap plots
to produce the plots use Alignment/OfflineValidation/python/runGCPTkAlMap.py 
'''

#TEST_COLOR_IDX = 2000
#TEST_COLOR = ROOT.TColor(TEST_COLOR_IDX, (0.)/255., (0.)/255., (255+0.)/255.)

KNOWN_VARIABLES = {
    'dr': {
        'name' : '#Deltar',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'dx': {
        'name' : '#Deltax',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'dy': {
        'name' : '#Deltay',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'dz': {
        'name' : '#Deltaz',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'rdphi': {
        'name' : 'r#Delta#phi',
        #'units': '#mum rad',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'dphi': {
        'name' : '#Delta#phi',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },
    'dalpha': {
        'name' : '#Delta#alpha',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },
    'dbeta': {
        'name' : '#Delta#beta',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },
    'dgamma': {
        'name' : '#Delta#gamma',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },
    'du': {
        'name' : '#Deltau',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'dv': {
        'name' : '#Deltav',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'dw': {
        'name' : '#Deltaw',
        'units': '#mum',
        'scale': 10000.,
        'range': [-200., 200.],
        },
    'da': {
        'name' : '#Deltaa',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },
    'db': {
        'name' : '#Deltab',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },
    'dg': {
        'name' : '#Deltag',
        'units': 'mrad',
        'scale': 1000.,
        'range': [-100., 100.],
        },  
}
EVENT_INFO = ' '

#ROOT.gStyle.SetLineScalePS(1)

def mean(data_list):
    return sum(data_list)/(len(data_list)+0.)
	
def StdDev(data_list):
    s2 = 0.
    m = mean(data_list)
    for point in data_list:
    	s2 += (point-m)**2
    return math.sqrt(s2/(len(data_list)+0.))

def read_TPLfile(file_name):
    o_file = open(file_name, 'r')
    lines = o_file.readlines()
    o_file.close()

    TPL_dict = {}
    for line in lines:
        if '#' in line: continue
        splt_line = line.replace('\n', '').split(' ')
        det_id = int(splt_line[0])
        x = []
        y = []
        for idx,coo in enumerate(splt_line[1:]):
            #print(coo)
            try:
                val = float(coo)
                if (idx%2) == 0: 
                    y.append(val)
                else: 
                    x.append(val)
            except ValueError:
                continue
        TPL_dict[det_id] = {}
        TPL_dict[det_id]['x'] = x
        TPL_dict[det_id]['y'] = y
    return TPL_dict

class TkAlMap:

    #def __init__(self, variable, title, root_file, two_sigma_cap=False, width=1500, height=800, GEO_file='TkMap_design_cfg.py', tracker='full'):
    def __init__(
            self, 
            variable, title, root_file,
            event_info=EVENT_INFO, 
            GEO_file='TkAlMapDesign_phase1_cfg.py', 
            height=1400, tracker='full', palette=2, 
            use_default_range=False, two_sigma_cap=False, do_tanh=False, check_tracker=True, 
            indicate_bad_modules=False, check_module_quality=False, skip_bad_modules=False,
            is_preliminary=True, is_simulation=False,
            sideway_title=False
            ):
        ROOT.gStyle.SetLineScalePS(1)

        # Configuration parameters
        self.GEO_file      = GEO_file
        self.tracker       = tracker
        self.width         = height 
        self.height        = height
        self.title         = title
        self.sideway_title = sideway_title
        self.event_info    = event_info
        self.default_range = use_default_range
        self.two_sigma_cap = two_sigma_cap
        self.root_file     = root_file
        self.do_tanh       = do_tanh
        self.do_bad_mod    = indicate_bad_modules
        self.check_mod_q   = check_module_quality
        self.skip_bad_mod  = skip_bad_modules
        self.is_pre        = is_preliminary
        self.is_sim        = is_simulation

        # Value Initialization
        self.max_val           = None
        self.min_val           = None
        self.color_bar_colors  = {}
        self.tree              = None
        self.is_cleaned        = False
        self.canvas            = None
        self.bad_modules       = []
        self.title_h           = 0

        # Internal parameters    
        #self.data_path = 'Alignment-OfflineValidation/TkAlMap/'
        self.data_path = 'Alignment/OfflineValidation/data/TkAlMap/'
        #self.data_path = 'Alignment/OfflineValidation/data/TkAlMap_inv/'
        #self.data_path = 'Alignment/OfflineValidation/data/TkAlMap_inv_xmin/'
        #self.data_path = 'Alignment/OfflineValidation/data/TkAlMap_inv_pix/'
        self.cfg_path = 'Alignment/OfflineValidation/python/TkAlMap_cfg/'

        # Colorbar stuff
        self.start_color_idx   = 1200
        self.n_color_color_bar = 1000
 
        # Initialization functions
        self.set_palette(palette)
        self.set_var(variable)
        self.load_tree()
        if check_tracker: self.detect_tracker_version()
        self.load_geometry()
        self.set_colorbar_colors()
        self.set_canvas()

    def set_var(self, var, units='cm', var_range=[None, None]):
        print('TkAlMap: setting variable to '+var)
        self.var = var
        self.var_name = var
        self.var_units = units
        self.var_scale = 1.
        self.var_min = var_range[0]
        self.var_max = var_range[1]
        if var in KNOWN_VARIABLES:
            self.var_name  = KNOWN_VARIABLES[var]['name']
            self.var_units = KNOWN_VARIABLES[var]['units']
            self.var_scale = KNOWN_VARIABLES[var]['scale']
            if self.var_min is None: self.var_min = KNOWN_VARIABLES[var]['range'][0]
            if self.var_max is None: self.var_max = KNOWN_VARIABLES[var]['range'][1]
        self.set_canvas()

    def set_canvas(self):
        canv_name = 'canvas_'+self.tracker+'_'+self.var
        if self.two_sigma_cap: canv_name += '_cap'
        #if not self.canvas is None: self.canvas.Destructor() => SegVio, Delete() is not allowed
        self.canvas = ROOT.TCanvas(canv_name, 'TkAlMap '+self.var+' canvas', self.width, self.height)
        print('Actual w: '+str(self.canvas.GetWw())+', Actual h: '+str(self.canvas.GetWh()))

#### Color setup
    def setup_colors(self):
        self.load_var()
        self.prepare_map_colors()
        self.fill_colors()
        if self.skip_bad_mod: self.mask_bad_modules()

    def prepare_map_colors(self):
        print('TkAlMap: preparing map colors')
        
        self.colors = []
        #self.palette = array('i', [])
        col_idx = self.start_color_idx + self.n_color_color_bar + 10
        self.col_dic = {}
        self.rgb_map = {}
        #pal_idx = 0
        #self.pal_map = {}
        for val in self.val_list:
            cap_val = val
            if cap_val > self.max_val: cap_val = self.max_val
            if cap_val < self.min_val: cap_val = self.min_val
            r, g, b = self.get_color_rgb(cap_val)
            idx = self.get_color_rgb_idx(cap_val)
            if idx in self.colors: continue
            self.colors.append(idx)
            col_idx +=1
            self.rgb_map[idx] = col_idx
            #print( idx, (r+0.)/255., (g+0.)/255., (b+0.)/255.)
            #color = ROOT.TColor(col_idx, (r+0.)/255., (g+0.)/255., (b+0.)/255.)

            #self.col_dic[idx] = ROOT.TColor(col_idx, (r+0.)/255., (g+0.)/255., (b+0.)/255.)
            try:
                col = ROOT.gROOT.GetColor(col_idx)
                col.SetRGB((r+0.)/255., (g+0.)/255., (b+0.)/255.)
                self.col_dic[idx] = col
            except:
                self.col_dic[idx] = ROOT.TColor(col_idx, (r+0.)/255., (g+0.)/255., (b+0.)/255., str(col_idx))
            #self.palette.append(col_idx)
        print('TkAlMap: map contains '+str(len(self.colors))+' colors')
     
    def set_palette(self, palette):
        self.palette = palette
        pal_str = 'TkAlMap: setting the palette to '+str(palette)
        if palette == 1: pal_str += ' (rainbow)'
        elif palette == 2: pal_str += ' (B->R diverging)'
        elif palette == 3: pal_str += ' (W->R diverging)'
        else: raise ValueError('TkAlMap: unkown palette value '+str(palette)+', allowed values are 1,2 and 3')
        print(pal_str)
        #ROOT.gstyle.SetPalette(len(self.colors), self.colors)
        #ROOT.gStyle.SetPalette(len(self.palette), self.palette)
        pass

    def get_color_rgb(self, val):
        if self.max_val is None or self.min_val is None:
            value_frac = val
        else:
            if self.do_tanh:
                val_th = math.tanh((val - self.mean_val)/(self.std_val))
                max_th = math.tanh((self.max_val - self.mean_val)/(self.std_val))
                min_th = math.tanh((self.min_val - self.mean_val)/(self.std_val))
                value_frac = (val_th - min_th + 0.)/(max_th - min_th)
            else:
                value_range = self.max_val - self.min_val
                if value_range == 0.: value_frac = 0.5
                else: value_frac = (val - self.min_val + 0.)/(value_range + 0.)

        if self.palette == 1:
            r = 255
            g = 255
            b = 255

            if value_frac < 0.25:
                r = 0
                g = int(255.*((value_frac/0.25)))
                b = 255         
            elif value_frac < 0.5:
                r = 0
                g = 255
                b = int(255.*(1. -(value_frac - 0.25)/0.25)) 
            elif value_frac < 0.75:
                r = int(255.*((value_frac - 0.5)/0.25))
                g = 255
                b = 0 
            else:
                r = 255 
                g = int(255.*(1. -(value_frac - 0.75)/0.25)) 
                b = 0
            return r, g, b 
        elif self.palette == 2:
            blue  = [59,   76, 192]
            red   = [180,   4,  38]
            white = [255, 255, 255] 
            r, g, b = DivergingColor(blue, red, white, value_frac) 
            return r, g, b
        elif self.palette == 3:
            #lred  = [255, 255, 255]
            red   = [120, 4, 45]
            white = [255, 255, 255] 
            r, g, b = DivergingColor(white, red, white, value_frac) 
            return r, g, b
        else: raise ValueError('TkAlMap: unkown palette value '+str(palette)+', allowed values are 1,2 and 3')

    def get_color_rgb_idx(self, val):
        r, g, b = self.get_color_rgb(val)
        #return r*1000000+g*1000+b+1000000000
        offset = 100
        return int(r*255*255 + g*255 + r + g + b + offset)

    def fill_colors(self):
        print('TkAlMap: filling the colors')
        #self.set_palette()
        for module in self.mod_val_dict:
            if module in self.TkAlMap_TPL_dict:
                if self.skip_bad_mod:
                    if module in self.bad_modules: continue
                val = self.mod_val_dict[module]
                cap_val = val
                if cap_val > self.max_val: cap_val = self.max_val
                if cap_val < self.min_val: cap_val = self.min_val
                rgb = self.get_color_rgb_idx(cap_val)
                col = self.rgb_map[rgb]
                #col = self.pal_map[rgb]
                #col = self.col_dic[rgb]
                #print(val, rgb, col)
                self.TkAlMap_TPL_dict[module].SetFillColor(col)
                #self.TkAlMap_TPL_dict[module].SetFillColor(TEST_COLOR_IDX)
            ####else: print('Warning: Unknown module '+str(module))

    def mask_bad_modules(self):
        n_bad = 0
        for module in self.bad_modules:
            if module in self.TkAlMap_TPL_dict:
                n_bad +=1
                self.TkAlMap_TPL_dict[module].SetFillColor(920+1) # kGray = 920
        print('TkAlMap: masked '+str(n_bad)+' modules')

    def set_colorbar_axis(self):
        print('TkAlMap: setting color bar axis')
        b_x1 = self.image_x1
        b_x2 = self.image_x2
        b_y1 = 0.06
        b_y2 = 0.06
        b_width = 0.01
        self.color_bar_axis = ROOT.TGaxis(b_x1, b_y1, b_x2, b_y2, self.min_val, self.max_val, 50510, '+S')
        self.color_bar_axis.SetName('color_bar_axis')
        self.color_bar_axis.SetLabelSize(0.02)
        self.color_bar_axis.SetTickSize(0.01)
        min_str = str(self.min_val)
        if min_str.endswith('.0'): min_str = min_str[:-2]
        max_str = str(self.max_val)
        if max_str.endswith('.0'): max_str = max_str[:-2]
        if self.two_sigma_cap and not self.default_range: self.color_bar_axis.SetTitle('{#mu - 2#sigma #leq '+self.var_name+' #leq #mu + 2#sigma} ('+self.var_units+')')
        #elif self.default_range: self.color_bar_axis.SetTitle('{'+min_str+' #leq '+self.var_name+' #leq '+max_str+'} ('+self.var_units+')')
        elif self.default_range: self.color_bar_axis.SetTitle(self.var_name+' ['+self.var_units+']')
        else: self.color_bar_axis.SetTitle(self.var_name+' ['+self.var_units+']')
        #self.color_bar_axis.SetTitleSize(0.025*self.text_factor)
        self.color_bar_axis.SetTitleSize(0.025)

    def set_colorbar_colors(self):
        print('TkAlMap: initialize color bar colors')
        if self.max_val is None or self.min_val is None:
            col_step = 1./(self.n_color_color_bar + 0.)
            val = col_step/2.
        else:
            b_range = self.max_val - self.min_val
            col_step = (b_range + 0.)/(self.n_color_color_bar + 0.)
            val = self.min_val + col_step/2.

        b_x1 = self.image_x1
        b_x2 = self.image_x2
        b_y1 = 0.06
        b_y2 = 0.06
        b_width = 0.01
        b_xrange = b_x2 - b_x1
        b_yrange = b_y2 - b_y1
        b_dx = (b_xrange + 0.)/(self.n_color_color_bar + 0.)
        b_dy = (b_yrange + 0.)/(self.n_color_color_bar + 0.)

        self.color_bar = {}
        x1 = b_x1
        y1 = b_y1
            
        col_idx = self.start_color_idx
        for i_c in range(self.n_color_color_bar):
            col_idx += 1
            r, g, b = self.get_color_rgb(val)
            try:
                col = ROOT.gROOT.GetColor(col_idx)
                col.SetRGB((r+0.)/255., (g+0.)/255., (b+0.)/255.)
                self.color_bar_colors[col_idx] = col
            except:
                self.color_bar_colors[col_idx] = ROOT.TColor(col_idx, (r+0.)/255., (g+0.)/255., (b+0.)/255., str(col_idx))
            x2 = x1 + b_dx 
            y2 = y1 + b_dy + b_width 
            x = array('d', [x1, x1, x2, x2])
            y = array('d', [y1, y2, y2, y1])
            self.color_bar[col_idx] = ROOT.TPolyLine(len(x), x, y)
            self.color_bar[col_idx].SetFillColor(col_idx)
            self.color_bar[col_idx].SetLineColor(col_idx)
 
            x1 += b_dx
            y1 += b_dy
            val += col_step

#### Load functions
    def load_tree(self):
        print('TkAlMap: loading tree ')
        tree_name = 'alignTree'
        r_file = ROOT.TFile(self.root_file)
        if r_file is None: raise ValueError('The file "'+self.root_file+'" could not be opened')

        tree_tmp = r_file.Get(tree_name)
        #self.tree = copy.deepcopy(tree_tmp)
        self.tmp_file_name = str(time.time()).replace('.', '_')+'_TkAlMapTempFile.root'
        self.tmp_file = ROOT.TFile(self.tmp_file_name, 'recreate')
        self.tree = tree_tmp.CloneTree()
        r_file.Close()
        self.is_cleaned = False

        if self.tree is None: raise ValueError('The tree "'+tree_name+'" was not found in file "'+self.root_file+'"')
        
    def load_bad_modules(self, in_file):
        o_file = open(in_file, 'r')
        lines = o_file.readlines()
        o_file.close()

        for line in lines:
            if '#' in line: continue
            mod = line.replace(' ', '').replace('\n', '')
            if mod == '': continue
            self.bad_modules.append(int(mod))

    def load_var(self):
        print('TkAlMap: loading variable values ')
        #tree_name = 'alignTree'
        #r_file = ROOT.TFile(self.root_file)
        #if r_file is None: raise ValueError('The file "'+self.root_file+'" could not be opened')

        #tree_tmp = r_file.Get(tree_name)
        #tree = copy.deepcopy(tree_tmp)
        #r_file.Close()

        #if tree is None: raise ValueError('The tree "'+tree_name+'" was not found in file "'+self.root_file+'"')

        self.mod_val_dict = {}
        self.val_list = []
        checked_mod_q = False
        has_mod_q = True
        for event in self.tree:
            if self.check_mod_q and not checked_mod_q:
                if not hasattr(event, 'badModuleQuality'): has_mod_q = False
                checked_mod_q = True
            module = event.id
            var = self.var
            if var == 'rdphi':
                val = getattr(event, 'r')*getattr(event, 'dphi')
            else: 
                val = getattr(event, var)
            val *= self.var_scale
            self.mod_val_dict[module] = val
            #if val not in self.val_list: self.val_list.append(val)
            #if module in self.TkAlMap_TPL_dict: self.val_list.append(val)
            ####else:
            ####    if 'full' in self.tracker:
            ####        print('Warning: Unknown module '+str(module))

            # check internal mod quality
            if self.check_mod_q and has_mod_q: 
                if getattr(event, 'badModuleQuality'): self.bad_modules.append(module)

            # skip bad modules if needed
            if self.skip_bad_mod:
                if not module in self.bad_modules:
                    if module in self.TkAlMap_TPL_dict: self.val_list.append(val)
            else:
                if module in self.TkAlMap_TPL_dict: self.val_list.append(val)
        #r_file.Close()
        if len(self.val_list) == 0:
            print('Warning: no values filled, 0 moduleId\'s matched')
            self.val_list = [-10+idx*0.5 for idx in range(41)]
        self.val_list.sort()
        self.mean_val = mean(self.val_list)
        self.std_val = StdDev(self.val_list)
        self.min_val = min(self.val_list) 
        self.max_val = max(self.val_list)
        
        if self.two_sigma_cap and not self.default_range:
           print('-- Capping max and min: ')
           print('---- True values   : '+str(self.max_val)+', '+str(self.min_val))
           self.min_val = max(min(self.val_list), self.mean_val - 2*self.std_val)
           self.max_val = min(max(self.val_list), self.mean_val + 2*self.std_val)
           print('---- Capped values : '+str(self.max_val)+', '+str(self.min_val))

        if self.default_range:
           #if not self.var in KNOWN_VARIABLES: print('Warning: capping to default range not possible for unknown variable "'+self.var+'"')
           if self.var_min is None or self.var_max is None: print('Warning: capping to default range for unknown variable "'+self.var+'" while range was not set is not possible')
           else:
               print('-- Capping max and min to default ranges: ')
               print('---- True values   : '+str(self.max_val)+', '+str(self.min_val))
               self.min_val = self.var_min
               self.max_val = self.var_max
               print('---- Capped values : '+str(self.max_val)+', '+str(self.min_val))

        if self.min_val == self.max_val:
            print('Warning: minimum value was equal to maximum value, '+str(self.max_val))
            self.min_val = self.mean_val - 1.
            self.max_val = self.mean_val + 1.

        #print(self.val_list)

    def detect_tracker_version(self):
        print('TkAlMap: detecting Tk version')
        #tree_name = 'alignTree'
        #r_file = ROOT.TFile(self.root_file)
        #if r_file is None: raise ValueError('The file "'+self.root_file+'" could not be opened')

        ##tree = r_file.Get(tree_name)
        #tree_tmp = r_file.Get(tree_name)
        #tree = copy.deepcopy(tree_tmp)
        #r_file.Close()

        #if tree is None: raise ValueError('The tree "'+tree_name+'" was not found in file "'+self.root_file+'"')
        phase = None
        for event in self.tree:
            module = event.id
            if module > 303040000 and module < 306450000:
                phase = 1
                break
            elif module > 302055000 and module < 302198000:
                phase = 0 
                break
        #r_file.Close()

        if phase is None: raise ValueError('TkAlMap: unknown tracker detected, is this phase2?')
        
        pahse_str = 'phase'+str(phase)
        print('TkAlMap: '+pahse_str+' tracker detected')
        if not pahse_str in self.GEO_file:
            print('TkAlMap: changing tracker to '+pahse_str+ ', if this is unwanted set "check_tracker" to False')
            self.GEO_file = 'TkAlMapDesign_'+pahse_str+'_cfg.py'
            #self.load_geometry()

    def load_geometry(self):
        source_path = os.getenv('CMSSW_BASE') + '/src/' 
        var = {}
        #execfile(source_path + self.cfg_path + self.GEO_file, var)
        exec(open(source_path + self.cfg_path + self.GEO_file).read(), var)

        MapStructure = var['TkMap_GEO']
        #MapStructure = TkMap_GEO
        
        all_modules = {}
        all_arrows = {}
        all_text = {}
        x_max = -9999.
        y_max = -9999.
        x_min = 9999.
        y_min = 9999.
        for det in MapStructure:
            if 'pixel' in self.tracker:
                if not 'pixel' in det: continue 
            elif 'strips' in self.tracker:
                if not 'strips' in det: continue 
            for sub in MapStructure[det]:
                for part in MapStructure[det][sub]:
                    if part == 'latex':
                        all_text[det+'_'+sub] = MapStructure[det][sub][part]
                        continue
                    if 'latex' in MapStructure[det][sub][part]:
                        all_text[det+'_'+sub+'_'+part] = MapStructure[det][sub][part]['latex']
                    if 'arrows' in MapStructure[det][sub][part]:
                        if 'full' in self.tracker:
                            # for full tracker only take arrows from strips
                            if 'strips' in det: all_arrows[det+'_'+sub+'_'+part] = MapStructure[det][sub][part]['arrows']
                        else: 
                            all_arrows[det+'_'+sub+'_'+part] = MapStructure[det][sub][part]['arrows']
                    TPL_file = source_path + self.data_path +MapStructure[det][sub][part]['file']
                    #TPL_file = cms.FileInPath(self.data_path +MapStructure[det][sub][part]['file'])
                    TPL_dict = read_TPLfile(TPL_file)
                    for module in TPL_dict:
                        x_canv = []
                        y_canv = []
                        for idx in range(len(TPL_dict[module]['x'])):
                            x_canv.append(TPL_dict[module]['x'][idx]*MapStructure[det][sub][part]['x_scale'] + MapStructure[det][sub][part]['x_off'])
                            y_canv.append(TPL_dict[module]['y'][idx]*MapStructure[det][sub][part]['y_scale'] + MapStructure[det][sub][part]['y_off'])
                        if max(x_canv) > x_max: x_max = max(x_canv)
                        if max(y_canv) > y_max: y_max = max(y_canv)
                        if min(x_canv) < x_min: x_min = min(x_canv)
                        if min(y_canv) < y_min: y_min = min(y_canv)
                        TPL_dict[module]['x'] = x_canv
                        TPL_dict[module]['y'] = y_canv
                    all_modules.update(TPL_dict)

        if not self.sideway_title:
            r_margin = 3
            l_margin = 3
            t_margin = 3 + 12
            #t_margin = 11
            b_margin = 3 + 5
        else:
            r_margin = 3
            l_margin = 3 + 8
            t_margin = 3
            b_margin = 3 + 5

        x_max += r_margin
        x_min -= l_margin
        y_max += t_margin
        y_min -= b_margin

        x_range = x_max - x_min
        y_range = y_max - y_min

        self.width = int(self.height*(x_range + 0.)/(y_range + 0.))
        self.canvas.SetWindowSize(self.width, self.height)

        if (x_range + 0.)/(self.width + 0.) > (y_range + 0.)/(self.height + 0.):
            x_scale = x_range
            y_scale = (self.height + 0.)/(self.width + 0.)*x_range
        else:
            y_scale = y_range
            x_scale = (self.width + 0.)/(self.height + 0.)*y_range

        # Set relative coordinates TPolyLines
        self.TkAlMap_TPL_dict = {} 
        for module in all_modules:
            x = array('d', [])
            y = array('d', [])
            for idx in range(len(all_modules[module]['x'])):
                x.append((all_modules[module]['x'][idx] - x_min + 0.)/(x_scale + 0.))
                y.append((all_modules[module]['y'][idx] - y_min + 0.)/(y_scale + 0.))
            # Begin point is end point
            x.append((all_modules[module]['x'][0] - x_min + 0.)/(x_scale + 0.))
            y.append((all_modules[module]['y'][0] - y_min + 0.)/(y_scale + 0.))
            #print(x, y)
            self.TkAlMap_TPL_dict[module] = ROOT.TPolyLine(len(x), x, y) 
            #self.TkAlMap_TPL_dict[module].SetFillColor(1)
            self.TkAlMap_TPL_dict[module].SetLineColor(1)
            #print('lineW', self.TkAlMap_TPL_dict[module].GetLineWidth())
            #self.TkAlMap_TPL_dict[module].Draw('f')
            #self.TkAlMap_TPL_dict[module].Draw()

        self.image_x1 = (l_margin + 0.)/(x_scale + 0.) 
        self.image_x2 = (x_max - r_margin - x_min + 0.)/(x_scale + 0.) 
        self.image_y1 = (b_margin + 0.)/(y_scale + 0.) 
        self.image_y2 = (y_max - t_margin - y_min + 0.)/(y_scale + 0.) 

        self.x_scale = x_scale
        self.y_scale = y_scale
        print('xy scale: ', x_scale, y_scale)
        self.text_factor = 85./(self.y_scale + 0.)
        if 'pixel' in self.tracker and 'phase0' in self.GEO_file: self.text_factor *= 2.
        #self.text_factor = 148./max(self.x_scale, self.y_scale)
        #self.text_factor = 1.

        # Set relative coordinates text
        #TL = ROOT.TLatex()
        #TL.SetTextSize(0.025)
        self.TkAlMap_text_dict = {}
        for key in all_text:
            x = (all_text[key]['x'] - x_min + 0.)/(x_scale + 0.)
            y = (all_text[key]['y'] - y_min + 0.)/(y_scale + 0.)
            self.TkAlMap_text_dict[key] = {}
            self.TkAlMap_text_dict[key]['x'] = x
            self.TkAlMap_text_dict[key]['y'] = y
            self.TkAlMap_text_dict[key]['alignment'] = all_text[key]['alignment']
            self.TkAlMap_text_dict[key]['text'] = all_text[key]['text']
            #TL.SetTextAlign(all_text[key]['alignment'])
            #TL.DrawLatex(x, y, all_text[key]['text'])

        # Set relative coordinates arrows and marks
        self.TkAlMap_arrow_dict = {}
        self.TkAlMap_arrow_marks_dict = {}
        for key in all_arrows:
            x_arr = copy.deepcopy(all_arrows[key][0])
            y_arr = copy.deepcopy(all_arrows[key][1])

            x_arr['x1'] = (all_arrows[key][0]['x1'] - x_min + 0.)/(x_scale + 0.)           
            x_arr['y1'] = (all_arrows[key][0]['y1'] - y_min + 0.)/(y_scale + 0.)           
            x_arr['x2'] = (all_arrows[key][0]['x2'] - x_min + 0.)/(x_scale + 0.)           
            x_arr['y2'] = (all_arrows[key][0]['y2'] - y_min + 0.)/(y_scale + 0.)           
            x_arr['label_pos'][0] = (all_arrows[key][0]['label_pos'][0] - x_min + 0.)/(x_scale + 0.)           
            x_arr['label_pos'][1] = (all_arrows[key][0]['label_pos'][1] - y_min + 0.)/(y_scale + 0.)           

            y_arr['x1'] = (all_arrows[key][1]['x1'] - x_min + 0.)/(x_scale + 0.)           
            y_arr['y1'] = (all_arrows[key][1]['y1'] - y_min + 0.)/(y_scale + 0.)           
            y_arr['x2'] = (all_arrows[key][1]['x2'] - x_min + 0.)/(x_scale + 0.)           
            y_arr['y2'] = (all_arrows[key][1]['y2'] - y_min + 0.)/(y_scale + 0.)           
            y_arr['label_pos'][0] = (all_arrows[key][1]['label_pos'][0] - x_min + 0.)/(x_scale + 0.)           
            y_arr['label_pos'][1] = (all_arrows[key][1]['label_pos'][1] - y_min + 0.)/(y_scale + 0.)           

            self.TkAlMap_arrow_dict[key] = {}
            self.TkAlMap_arrow_dict[key]['rel_arr'] = [x_arr, y_arr]
            self.TkAlMap_arrow_dict[key]['x_arr'] = ROOT.TArrow(x_arr['x1'], x_arr['y1'], x_arr['x2'], x_arr['y2'], 0.05, '|>')
            self.TkAlMap_arrow_dict[key]['y_arr'] = ROOT.TArrow(y_arr['x1'], y_arr['y1'], y_arr['x2'], y_arr['y2'], 0.05, '|>')

            # Marks
            self.TkAlMap_arrow_marks_dict[key] = []
            for arr in all_arrows[key]:
                if not 'marks' in arr: continue
                for mark in arr['marks']:
                    tmp_mark = copy.deepcopy(mark)
                    tmp_mark['x1'] = (mark['x1'] - x_min + 0.)/(x_scale + 0.) 
                    tmp_mark['x2'] = (mark['x2'] - x_min + 0.)/(x_scale + 0.) 
                    tmp_mark['y1'] = (mark['y1'] - y_min + 0.)/(y_scale + 0.) 
                    tmp_mark['y2'] = (mark['y2'] - y_min + 0.)/(y_scale + 0.)
                    tmp_mark['label_pos'][0] = (mark['label_pos'][0] - x_min + 0.)/(x_scale + 0.)
                    tmp_mark['label_pos'][1] = (mark['label_pos'][1] - y_min + 0.)/(y_scale + 0.)

                    line_dict = {}
                    line_dict['line'] = ROOT.TLine(tmp_mark['x1'], tmp_mark['y1'], tmp_mark['x2'], tmp_mark['y2'])
                    line_dict['label'] = mark['label'] 
                    line_dict['alignment'] = mark['alignment'] 
                    line_dict['label_pos'] = tmp_mark['label_pos']
                    self.TkAlMap_arrow_marks_dict[key].append(line_dict)
                     

#### Titles and info
    def draw_title(self):
        TL = ROOT.TLatex()
        #TL.SetTextSize(0.035)
        TL.SetTextSize(0.03*self.text_factor)
        TL.SetTextFont(42)
        #TL.SetTextFont(40)

        # Extract title height
        w_t = ctypes.c_uint(0)
        h_t = ctypes.c_uint(0)
        TL.GetTextExtent(w_t, h_t, self.title)
        split_count = 1
        if '#splitline' in self.title: split_count = len(self.title.split('#splitline'))
        self.title_h = split_count*int(h_t.value) 

        if not self.sideway_title:
            TL.SetTextAlign(13)
            x1 = self.image_x1
            if self.tracker == 'full': y1 = 1-(5/(self.y_scale+0.))
            elif 'pixel' in self.tracker and 'phase0' in self.GEO_file: y1 = 1-(6/(self.y_scale+0.))
            else: y1 = 1-(4./(self.y_scale+0.))
        else:
            TL.SetTextAngle(90)
           
            x1 = self.image_x1 - (self.title_h+0.)/(self.width+0.) - 3./(self.x_scale+0.)
            y1 = self.image_y2
            TL.SetTextAlign(33) #33
        self.canvas.cd()
        TL.DrawLatex(x1, y1, self.title)
         
    def draw_cms_prelim(self):
        TL = ROOT.TLatex()
        #factor = 1. / 0.82
        #TL.SetTextSize(0.035*factor)
        TL.SetTextSize(0.0427*self.text_factor)
        TL.SetTextFont(61)

        if not self.sideway_title:
            TL.SetTextAlign(11)

            w_cms = ctypes.c_uint(0)
            h_cms = ctypes.c_uint(0)
            TL.GetTextExtent(w_cms, h_cms, 'CMS')
            x1 = self.image_x1
            y1 = 1. - (h_cms.value+0.)/(self.height+0.) - (1./(self.y_scale+0.))
            self.canvas.cd()
            TL.DrawLatex(x1, y1, 'CMS')
  
            TL.SetTextSize(0.035*self.text_factor)
            TL.SetTextFont(42)
            x1_prel = x1 + 1.1*(w_cms.value+0.)/(self.width+0.)
            if self.is_pre: TL.DrawLatex(x1_prel, y1, '#it{Preliminary}')
            elif self.is_sim: TL.DrawLatex(x1_prel, y1, '#it{Simulation}')        

            self.draw_event_info(y1)
        else:
            TL.SetTextAngle(90)
            TL.SetTextAlign(33)
            #TL.SetTextAlign(31)
            #TL.SetTextAlign(13)
            
            w_cms = ctypes.c_uint(0)
            h_cms = ctypes.c_uint(0)
            TL.GetTextExtent(w_cms, h_cms, 'CMS')
            x1 = self.image_x1 - (self.title_h+0.)/(self.width+0.) - 3./(self.x_scale+0.)
            y1 = self.image_y1 
            self.canvas.cd()
            TL.DrawLatex(x1, y1, 'CMS')

            TL.SetTextSize(0.035*self.text_factor)
            TL.SetTextFont(42)
            y1_prel = y1 + 1.1*(h_cms.value+0.)/(self.height+0.)
            if self.is_pre: TL.DrawLatex(x1_prel, y1, '#it{Preliminary}')
            elif self.is_sim: TL.DrawLatex(x1_prel, y1, '#it{Simulation}')        
            

    def draw_event_info(self, y):
        TL = ROOT.TLatex()
        TL.SetTextSize(0.035*self.text_factor)
        TL.SetTextFont(42)

        if 'pixel' in self.tracker and 'phase0' in self.GEO_file:
            TL.SetTextAlign(13)
            x1 = self.image_x1
            y1 = y - (0.4/(self.y_scale+0.))
        else:
            TL.SetTextAlign(31)
            x1 = self.image_x2
            y1 = y
        self.canvas.cd()
        TL.DrawLatex(x1, y1, self.event_info)

    def draw_all_arrows(self):
        #self.canvas.cd()
        TL = ROOT.TLatex()
        #print('scales: '+str(self.x_scale)+','+str(self.y_scale))
        TL.SetTextSize(0.020*self.text_factor)
        for key in self.TkAlMap_arrow_dict:
            self.TkAlMap_arrow_dict[key]['x_arr'].SetArrowSize(0.006) 
            self.TkAlMap_arrow_dict[key]['y_arr'].SetArrowSize(0.006)
            self.TkAlMap_arrow_dict[key]['x_arr'].SetLineWidth(3) 
            self.TkAlMap_arrow_dict[key]['y_arr'].SetLineWidth(3)
            self.TkAlMap_arrow_dict[key]['x_arr'].SetOption('|>') 
            self.TkAlMap_arrow_dict[key]['y_arr'].SetOption('|>')
            self.TkAlMap_arrow_dict[key]['x_arr'].Draw() 
            self.TkAlMap_arrow_dict[key]['y_arr'].Draw()
            x_arr_d = self.TkAlMap_arrow_dict[key]['rel_arr'][0]
            y_arr_d = self.TkAlMap_arrow_dict[key]['rel_arr'][1]
            TL.SetTextAlign(x_arr_d['alignment'])
            TL.DrawLatex(x_arr_d['label_pos'][0], x_arr_d['label_pos'][1], x_arr_d['label'])
            TL.SetTextAlign(y_arr_d['alignment'])
            TL.DrawLatex(y_arr_d['label_pos'][0], y_arr_d['label_pos'][1], y_arr_d['label'])

        TL.SetTextSize(0.015*self.text_factor)
        for key in self.TkAlMap_arrow_dict:
            # Draw marks
            for mark in self.TkAlMap_arrow_marks_dict[key]:
                mark['line'].SetLineWidth(3)
                mark['line'].Draw()
                TL.SetTextAlign(mark['alignment'])
                TL.DrawLatex(mark['label_pos'][0], mark['label_pos'][1], mark['label'])

    def indicate_bad_modules(self):
        bad_col_idx = self.start_color_idx - 1
        #bad_col_rgb = [85, 148, 146] #=> grey-ish cyan-ish visible enough?
        bad_col_rgb = [223, 223, 87] #=> grey-ish yellow visible enough?
        try:
            col = ROOT.gROOT.GetColor(bad_col_idx)
            col.SetRGB((bad_col_rgb[0]+0.)/255., (bad_col_rgb[1]+0.)/255., (bad_col_rgb[2]+0.)/255.)
            self.bad_col = col
        except:
            self.bad_col = ROOT.TColor(bad_col_idx, (bad_col_rgb[0]+0.)/255., (bad_col_rgb[1]+0.)/255., (bad_col_rgb[2]+0.)/255., str(bad_col_idx))
        
        n_bad = 0
        for module in self.bad_modules:
            if module in self.TkAlMap_TPL_dict:
               n_bad += 1
               #self.TkAlMap_TPL_dict[module].SetFillColor(1) 
               self.TkAlMap_TPL_dict[module].SetLineColor(bad_col_idx) 
               #self.TkAlMap_TPL_dict[module].SetLineStyle(2) => lines too small to see effect 
               self.TkAlMap_TPL_dict[module].SetLineWidth(6)
               self.TkAlMap_TPL_dict[module].Draw()
        print('TkAlMap: indicated '+str(n_bad)+' bad modules')

#### Draw functions
    def draw_text(self):
        print('TkAlMap: drawing text')
        self.canvas.cd()
        TL = ROOT.TLatex()
        TL.SetTextSize(0.025*self.text_factor)
        for key in self.TkAlMap_text_dict:
            TL.SetTextAlign(self.TkAlMap_text_dict[key]['alignment'])
            TL.DrawLatex(self.TkAlMap_text_dict[key]['x'], self.TkAlMap_text_dict[key]['y'], self.TkAlMap_text_dict[key]['text'])
        self.draw_title()
        self.draw_cms_prelim()
        self.canvas.Update()

    def draw_TPL(self):
        print('TkAlMap: drawing PolyLines')
        self.canvas.cd()
        for module in self.TkAlMap_TPL_dict:
            self.TkAlMap_TPL_dict[module].Draw('f')
            self.TkAlMap_TPL_dict[module].SetLineWidth(1) 
            self.TkAlMap_TPL_dict[module].Draw()
        self.canvas.Update()

    def draw_color_bar(self):
        print('TkAlMap: drawing color bar')
        self.canvas.cd()
        for box in self.color_bar:
            self.color_bar[box].Draw('f')
            #self.color_bar[box].Draw()
        self.color_bar_axis.Draw()
        self.canvas.Update()

    def save(self, out_dir='.', extension='pdf'):
        name = '_'.join(['TkAlMap', self.tracker, self.var])
        if self.sideway_title:
            name += '_ST'
        if self.two_sigma_cap and not self.default_range:
            name += '_4sig'
        elif self.default_range:
            name += '_drange'
        path = out_dir + '/' + name + '.' + extension
        print('TkAlMap: saving canvas in "'+path+'"')
        self.canvas.SaveAs(path)


#### Do all
    def analyse(self):
        self.setup_colors()
        self.set_colorbar_axis()
        if self.do_tanh: self.set_colorbar_colors()
        self.draw_all_arrows()
        self.draw_TPL()
        self.draw_text()
        self.draw_color_bar()
        if self.do_bad_mod: self.indicate_bad_modules()

### Test functions
    def plot_variable_distribution(self, nbins=200, out_dir='.', show_10sigma=False):
        print('TkAlMap: drawing variable distribution')
        canv_name = 'histogram_canvas_'+self.tracker+'_'+self.var
        if self.two_sigma_cap: canv_name += '_cap'
        canvas = ROOT.TCanvas(canv_name, 'TkAlMap '+self.var+' histogram canvas', 800, 800)
 
        if show_10sigma:
            h_min = self.mean_val - 5*self.std_val
            h_max = self.mean_val + 5*self.std_val
        else:
            h_min = min(min(self.val_list), self.mean_val - 2*self.std_val) - self.std_val
            h_max = max(max(self.val_list), self.mean_val + 2*self.std_val) + self.std_val
        hist = ROOT.TH1F(self.var+'_hist', 'Variable distribution', nbins, h_min, h_max)
        for val in self.val_list:
            hist.Fill(val)
        hist.GetXaxis().SetTitle(self.var_name+' ['+self.var_units+']')
        hist.GetYaxis().SetTitle('modules')
        ROOT.gStyle.SetOptStat(0)
        hist.Draw('e1')
        canvas.Update()
        left = ROOT.TLine(self.mean_val - 2*self.std_val, canvas.GetUymin(), self.mean_val - 2*self.std_val, canvas.GetUymax())
        left.SetLineColor(2) 
        left.SetLineStyle(9) 
        left.Draw() 
        right = ROOT.TLine(self.mean_val + 2*self.std_val, canvas.GetUymin(), self.mean_val + 2*self.std_val, canvas.GetUymax())
        right.SetLineColor(2) 
        right.SetLineStyle(9) 
        right.Draw() 
        mid = ROOT.TLine(self.mean_val, canvas.GetUymin(), self.mean_val, canvas.GetUymax())
        mid.SetLineColor(1)
        mid.SetLineStyle(9) 
        mid.Draw()
        canvas.Update()
        name = '_'.join(['VariableDistribution', self.var, self.tracker])
        path = out_dir + '/' + name + '.png' 
        canvas.SaveAs(path)

    def get_n_outliners(self, n=100, out_dir='.'):
        print('TkAlMap: gathering first '+str(n)+' outliners')
        mod_l = []
        std_l = []
        for module in self.mod_val_dict:
            if module in self.TkAlMap_TPL_dict:
                val = self.mod_val_dict[module]
                n_std = abs((val - self.mean_val + 0.)/(self.std_val + 0.))
                std_l.append(n_std)
                mod_l.append(module)

        std_l_s, mod_l_s = (list(t) for t in zip(*sorted(zip(std_l, mod_l))))
        name = '_'.join(['Outliners', 'n'+str(n), self.var, self.tracker])
        path = out_dir + '/' +name+ '.txt'
        o_file = open(path, 'w')
        o_file.write('#moduleID\t#stdDev\n')
        idx = -1
        while abs(idx)<=n and abs(idx)<=len(mod_l):
            o_file.write(str(mod_l_s[idx])+'\t'+str(std_l_s[idx])+'\n')
            idx -= 1
        o_file.close()
        print('TkAlMap: '+path+' created')

### Clean up
    def clean_up(self):
        if not self.is_cleaned:
            print('TkAlMap: deleting temporary file "'+self.tmp_file_name+'"')
            self.tmp_file.Close()
            os.remove(self.tmp_file_name)
            self.is_cleaned = True

    def __del__(self):
        self.clean_up()


if __name__ == '__main__':

    def parser():
        parser = argparse.ArgumentParser(description = "TkAlMap parser for custom use", formatter_class=argparse.RawTextHelpFormatter)
        parser.add_argument("rootfile", metavar='rootfile', type=str, action="store", help="Input root file containing TTree \"alignTree\"")
        parser.add_argument("variable", metavar='variable', type=str, action="store", help="name of variable to plot")
        parser.add_argument("-o", "--out-dir", type=str, action = "store", default = '.', help ="Output directory whare plots will be stored")
        parser.add_argument("-d", "--drange", action = "store_true", help ="Set default range (only for known variables)")
        parser.add_argument("-s", "--s-cap", action = "store_true", help ="Cap values between 4 sigma around mean")
        parser.add_argument("-p", "--palette", type=int, action = "store", default = 2, choices = [1, 2, 3], help ="Set color palette (1 rainbow, 2 diverging color B->R, 3 diverging color W->R)")
        parser.add_argument("-t", "--tracker", action = "store", default = "full", choices = ["full", "pixel", "strips"], help ="Detector parts to show")
        parser.add_argument("--phase", type=int, action = "store", default = 1, choices = [0, 1], help ="Set Phase (upgrade version) of detector to plot.")
        parser.add_argument("--tanh", action = "store_true", help ="Rescale color range to tanh(val/2*sigma)")
        parser.add_argument("--check-tk", action = "store_true", help ="Auto detect tracker phase (this will overwrite --phase option)")
        parser.add_argument("--show-bad", action = "store_true", help ="Indicate bad modules with different edge color")
        parser.add_argument("--skip-bad", action = "store_true", help ="Exclude bad module from beeing evaluated")
        parser.add_argument("--bad-modules", type=str, action = "store", default = '', help ="Pass txt file with bad module ID's")
        parser.add_argument("--var-dist", action = "store_true", help ="Plot variable distribution")
        parser.add_argument("--outliners", action = "store_true", help ="Collect outliners")
        parser.add_argument("--title", type=str, action = "store", default = None, help ="Title in plot")
        parser.add_argument("--event-info", type=str, action = "store", default = None, help ="Event info in top right corner")
        parser.add_argument("--units", type=str, action = "store", default = "cm", help ="Units to set on the color bar axis")
        parser.add_argument("--max", type=str, action = "store", default = None, help ="Set max value to show")
        parser.add_argument("--min", type=str, action = "store", default = None, help ="Set min value to show")
        parser.add_argument("-b", "--batch", action = "store_true", help ="Set batch mode")
        parser.add_argument("--no-preliminary", action = "store_false", help ="Remove Preliminary in top left corner")
        parser.add_argument("--simulation"    , action = "store_true", help ="Set Simulation in top left corner if not Preliminary")
        parser.add_argument("--sideway-title" , action = "store_true", help ="Put tile to the left side")
    
        return parser.parse_args()

    args = parser()
    ROOT.gROOT.SetBatch(args.batch);

    #ROOT.gROOT.SetBatch(args.batch)
    det_phase = 'TkAlMapDesign_phase1_cfg.py' 
    if args.phase == 0: det_phase = 'TkAlMapDesign_phase0_cfg.py'
    elif args.phase == 1: det_phase = 'TkAlMapDesign_phase1_cfg.py'

    title = 'TkAlMap: '+args.variable
    if not args.title is None: title = args.title

    event_info = EVENT_INFO
    if not args.event_info is None: event_info = args.event_info


    TkMap = TkAlMap(
        args.variable, 
        title, 
        args.rootfile,
        event_info=event_info, 
        use_default_range=args.drange, 
        two_sigma_cap=args.s_cap, 
        tracker=args.tracker, 
        palette=args.palette, 
        do_tanh=args.tanh, 
        check_tracker=args.check_tk, 
        GEO_file=det_phase, 
        indicate_bad_modules=args.show_bad,
        skip_bad_modules=args.skip_bad,
        is_preliminary=args.no_preliminary,
        is_simulation=args.simulation,
        sideway_title=args.sideway_title
        )

    TkMap.set_var(args.variable, units=args.units) 
    if not args.max is None and not args.min is None: TkMap.set_var(args.variable, units=args.units, var_range=[float(args.min), float(args.max)]) 
    #def __init__(self, variable, title, root_file, use_default_range=False, two_sigma_cap=False, height=1400, GEO_file='TkAlMapDesign_phase1_cfg.py', tracker='full', palette=2, do_tanh=False, check_tracker=True):

    if args.bad_modules != '': TkMap.load_bad_modules(args.bad_modules)

    TkMap.analyse()
    TkMap.save(out_dir=args.out_dir)
    TkMap.save(out_dir=args.out_dir, extension='png')
    if args.var_dist: TkMap.plot_variable_distribution(out_dir=args.out_dir)
    if args.outliners: TkMap.get_n_outliners(out_dir=args.out_dir)
