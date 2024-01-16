library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
 
entity bin_to_7_seg is port 
( 
	bin: in std_logic_vector(3 downto 0);
	seg: out std_logic_vector(7 downto 0)
 );
 
end bin_to_7_seg;

architecture Behavioral of bin_to_7_seg is

begin

process(bin) is
    begin
  
        case bin is
            when "0000" =>
                 seg <= "11000000";
            when "0001" =>
                 seg <= "11111001";
            when "0010" =>
                 seg <= "10100100";
            when "0011" =>
						seg <= "10110000";
            when "0100" =>
						seg <= "10011001";
            when "0101" =>
						seg <= "10010010";
            when "0110" =>
						seg <= "10000010";
            when "0111" =>
						seg <= "11111000";
				when "1000" =>
						seg <= "10000000";
				when "1001" =>
						seg <= "10010000";
            when others =>
                seg <= "11111111";
        end case;
end process;

end Behavioral;