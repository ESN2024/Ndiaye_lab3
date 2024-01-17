library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
 
entity bin_to_7_seg_virgule is port 
( 
	bin: in std_logic_vector(3 downto 0);
	seg: out std_logic_vector(7 downto 0)
 );
 
end bin_to_7_seg_virgule;

architecture Behavioral of bin_to_7_seg_virgule is

begin

process(bin) is
    begin
  
        case bin is
            when "0000" =>
                 seg <= "01000000";
            when "0001" =>
                 seg <= "01111001";
            when "0010" =>
                 seg <= "00100100";
            when "0011" =>
						seg <= "00110000";
            when "0100" =>
						seg <= "00011001";
            when "0101" =>
						seg <= "00010010";
            when "0110" =>
						seg <= "00000010";
            when "0111" =>
						seg <= "01111000";
				when "1000" =>
						seg <= "00000000";
				when "1001" =>
						seg <= "00010000";
            when others =>
                seg <= "01111111";
        end case;
end process;

end Behavioral;