library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library UNISIM;
use UNISIM.VComponents.all;

entity i2c_master_rtc is
    Generic (
        SYS_CLK_FREQ : integer := 100_000_000;
        I2C_FREQ     : integer := 100_000
    );
    Port (
        clk        : in  std_logic;
        rst_n      : in  std_logic;
        ctrl_reg   : in  std_logic_vector(31 downto 0);
        status_reg : out std_logic_vector(31 downto 0);
        data_in    : in  std_logic_vector(7 downto 0);
        data_out   : out std_logic_vector(7 downto 0);
        sda        : inout std_logic;
        scl        : inout std_logic
    );
end i2c_master_rtc;

architecture rtl of i2c_master_rtc is

    constant DIV_VAL : integer := (SYS_CLK_FREQ / I2C_FREQ) / 4;

    signal clk_cnt : integer range 0 to DIV_VAL := 0;
    signal tick    : std_logic := '0';

    type state_t is (IDLE, START, WRITE_BYTE, READ_BYTE, WAIT_ACK, SEND_ACK, STOP, DONE);
    signal state : state_t := IDLE;

    signal sda_oe, scl_oe : std_logic := '0';
    signal sda_t, scl_t   : std_logic;
    signal sda_i          : std_logic;

    signal shift_reg : std_logic_vector(7 downto 0) := (others => '0');
    signal bit_cnt   : integer range 0 to 7 := 7;
    signal rx_ack    : std_logic := '1';

    signal busy_flag : std_logic := '0';
    signal done_flag : std_logic := '0';

    alias cmd_en    : std_logic is ctrl_reg(0);
    alias cmd_start : std_logic is ctrl_reg(1);
    alias cmd_stop  : std_logic is ctrl_reg(2);
    alias cmd_write : std_logic is ctrl_reg(3);
    alias cmd_read  : std_logic is ctrl_reg(4);
    alias cfg_ack   : std_logic is ctrl_reg(5);

begin

    -- ===============================
    -- IOBUF FIX (VHDL-93 SAFE)
    -- ===============================
    sda_t <= not sda_oe;
    scl_t <= not scl_oe;

    IOBUF_SDA : IOBUF
    port map (
        O  => sda_i,
        IO => sda,
        I  => '0',
        T  => sda_t
    );

    IOBUF_SCL : IOBUF
    port map (
        O  => open,
        IO => scl,
        I  => '0',
        T  => scl_t
    );

    -- ===============================
    -- STATUS FLAGS
    -- ===============================
    process(state)
    begin
        if state /= IDLE then
            busy_flag <= '1';
        else
            busy_flag <= '0';
        end if;

        if state = DONE then
            done_flag <= '1';
        else
            done_flag <= '0';
        end if;
    end process;

    status_reg <= (31 downto 3 => '0') &
                  rx_ack &
                  busy_flag &
                  done_flag;

    -- ===============================
    -- CLOCK DIVIDER
    -- ===============================
    process(clk)
    begin
        if rising_edge(clk) then
            if clk_cnt = DIV_VAL-1 then
                clk_cnt <= 0;
                tick <= '1';
            else
                clk_cnt <= clk_cnt + 1;
                tick <= '0';
            end if;
        end if;
    end process;

    -- ===============================
    -- I2C FSM
    -- ===============================
    process(clk)
    begin
        if rising_edge(clk) then
            if rst_n = '0' or cmd_en = '0' then
                state  <= IDLE;
                sda_oe <= '0';
                scl_oe <= '0';
                bit_cnt <= 7;

            elsif tick = '1' then
                case state is

                    when IDLE =>
                        if cmd_start = '1' then
                            state <= START;
                        elsif cmd_write = '1' then
                            shift_reg <= data_in;
                            bit_cnt <= 7;
                            state <= WRITE_BYTE;
                        elsif cmd_read = '1' then
                            bit_cnt <= 7;
                            state <= READ_BYTE;
                        elsif cmd_stop = '1' then
                            state <= STOP;
                        end if;

                    when START =>
                        sda_oe <= '1';
                        scl_oe <= '0';
                        state <= DONE;

                    when WRITE_BYTE =>
                        sda_oe <= shift_reg(bit_cnt);
                        scl_oe <= '1';
                        scl_oe <= '0';
                        if bit_cnt = 0 then
                            state <= WAIT_ACK;
                        else
                            bit_cnt <= bit_cnt - 1;
                        end if;

                    when WAIT_ACK =>
                        sda_oe <= '0';
                        rx_ack <= sda_i;
                        state <= DONE;

                    when READ_BYTE =>
                        sda_oe <= '0';
                        shift_reg(bit_cnt) <= sda_i;
                        if bit_cnt = 0 then
                            state <= SEND_ACK;
                        else
                            bit_cnt <= bit_cnt - 1;
                        end if;

                    when SEND_ACK =>
                        sda_oe <= not cfg_ack;
                        data_out <= shift_reg;
                        state <= DONE;

                    when STOP =>
                        sda_oe <= '0';
                        scl_oe <= '0';
                        state <= DONE;

                    when DONE =>
                        state <= IDLE;

                    when others =>
                        state <= IDLE;

                end case;
            end if;
        end if;
    end process;

end rtl;
