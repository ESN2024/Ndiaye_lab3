library ieee;
use ieee.std_logic_1164.all;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

entity lab3_vhd is port
(
	clk : in    std_logic;
	rst : in    std_logic;
	opencores_i2c_0_export_0_scl_pad_io : inout std_logic; 
	opencores_i2c_0_export_0_sda_pad_io : inout std_logic; 
	bouton : in    std_logic ; 
	seg5    : out   std_logic_vector(7 downto 0);       
	seg4    : out   std_logic_vector(7 downto 0);
	seg3    : out   std_logic_vector(7 downto 0);       
	seg2    : out   std_logic_vector(7 downto 0);       
	seg1    : out   std_logic_vector(7 downto 0)        

);

end entity;

architecture rtl of lab3_vhd is

signal data1 : std_logic_vector(3 downto 0);
signal data2 : std_logic_vector(3 downto 0);
signal data3 : std_logic_vector(3 downto 0);
signal data4 : std_logic_vector(3 downto 0);
signal data5 : std_logic;

    component lab3 is port 
	 (
		clk_clk                             : in    std_logic                    := 'X'; -- clk
		reset_reset_n                       : in    std_logic                    := 'X'; -- reset_n
		opencores_i2c_0_export_0_scl_pad_io : inout std_logic                    := 'X'; -- scl_pad_io
		opencores_i2c_0_export_0_sda_pad_io : inout std_logic                    := 'X'; -- sda_pad_io
		bouton_export                       : in    std_logic                    := 'X'; -- export
		pio_5_external_connection_export    : out   std_logic;        -- export
      pio_4_external_connection_export    : out   std_logic_vector(3 downto 0);         -- export  
		pio_3_external_connection_export    : out   std_logic_vector(3 downto 0);        -- export
		pio_2_external_connection_export    : out   std_logic_vector(3 downto 0);        -- export
		pio_1_external_connection_export    : out   std_logic_vector(3 downto 0)         -- export
     );
    end component lab3;
	 
	 component bin_to_7_seg is port 
	 (
					bin: in std_logic_vector(3 downto 0);
					seg: out std_logic_vector(7 downto 0)
     );
    end component bin_to_7_seg;
	 
	 component bin_to_7_seg_virgule is port 
	 (
					bin: in std_logic_vector(3 downto 0);
					seg: out std_logic_vector(7 downto 0)
     );
    end component bin_to_7_seg_virgule;
	 
begin

    u0 : component lab3 port map 
		  (
            clk_clk                             => clk,                            
            reset_reset_n                       => rst,                      
            opencores_i2c_0_export_0_scl_pad_io => opencores_i2c_0_export_0_scl_pad_io, 
            opencores_i2c_0_export_0_sda_pad_io => opencores_i2c_0_export_0_sda_pad_io,                          
            bouton_export                       => bouton,                                   
            pio_5_external_connection_export    => data5,    
            pio_4_external_connection_export    => data4,                                        
            pio_3_external_connection_export    => data3,    
            pio_2_external_connection_export    => data2,    
            pio_1_external_connection_export    => data1     
        );
		  
	v0: component bin_to_7_seg port map(data1,seg1);

	v1: component bin_to_7_seg port map(data2,seg2);

	v2: component bin_to_7_seg_virgule port map(data3,seg3);
	
	v3: component bin_to_7_seg port map(data4,seg4);

	process(data5) is
	begin
		case data5 is
			when '1' =>
				seg5 <= "11111111";
			when '0' =>
				seg5 <= "10111111";
		end case;
	end process;
	
 
end rtl;