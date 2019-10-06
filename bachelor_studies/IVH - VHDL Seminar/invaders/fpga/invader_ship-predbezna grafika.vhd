library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;
use work.vga_controller_cfg.all;
use work.clkgen_cfg.all;

--Inspirovane aplikaciu Digger, mozno bude neskor pouzite ako lod invaderov   
architecture behav of tlv_pc_ifc is 
  signal vga_mode: std_logic_vector(60 downto 0);
  signal red: std_logic_vector(2 downto 0);
  signal green: std_logic_vector(2 downto 0);
  signal blue: std_logic_vector(2 downto 0);
  signal rgb : std_logic_vector(8 downto 0);
  signal rgbf : std_logic_vector(8 downto 0);
  signal vgaRow: std_logic_vector(11 downto 0);
  signal vgaCol: std_logic_vector(11 downto 0);

  signal invader_clr : std_logic_vector(0 to 1);
  signal invader_idx : integer range 0 to 4 := 0;
  signal rom_col : integer range 0 to 63;
  
  signal en_1MHz, en_8Hz, en_50Hz : std_logic;
 
  type pamet is array(0 to 16*1-1) of std_logic_vector(0 to 31);

  signal rom_invader: pamet := 
            ("00000000000000000000000000000000",
			       "00000000000000000000000000000000",
             "00000010010100010100010110000000", --(0)
             "00000010010101010101010110000000",
             "00000010111111111111111110000000",
			       "00000010111111111111111110000000",
			       "00000000101011111111101000000000",
			       "00000000001011111111100000000000",
			       "00000000001011111111100000000000",
			       "00000000001011111111100000000000",
			       "00000000101111111111111000000000",
			       "00000000101111111111111000000000",
			       "00000000001010111110100000000000",
			       "00000000000010101010000000000000",
			       "00000000000000000000000000000000",
                               (others => '0'));
         
begin

vga: entity work.vga_controller(arch_vga_controller)
  port map(
    CLK => CLK,
    RST => RESET,
    ENABLE => '1',
    MODE => vga_mode,
    DATA_RED => red,
    DATA_GREEN => green,
    DATA_BLUE => blue,
    ADDR_COLUMN => vgaCol,
    ADDR_ROW => vgaRow,
    VGA_RED => RED_V,
    VGA_GREEN => GREEN_V,
    VGA_BLUE => BLUE_V,
    VGA_HSYNC => HSYNC_V,
    VGA_VSYNC => VSYNC_V
  );
  
setmode(r640x480x60, vga_mode);

rom_col <= conv_integer(vgaCol(3 downto 0)) * 2;
invader_clr <= rom_invader(invader_idx*16 + conv_integer(vgaRow(4 downto 0)))(rom_col to rom_col+1);

rgb <= "111"&"000"&"000" when invader_clr="01" else      --farba motora - cervena
       "111"&"111"&"111" when invader_clr="10" else      --farba stitu - biela
       "100"&"100"&"100" when invader_clr="11" else      --farba lode  - siva
       "000"&"000"&"000";                               --farba pozadia - cierna

rgbf <= rgb when(vgaCol(11 downto 4) = "00001111") and (vgaRow(11 downto 4) = "00001100") else "000"&"000"&"000";  

red <= rgbf(8 downto 6);
green <= rgbf(5 downto 3);
blue <= rgbf(2 downto 0);

end;
