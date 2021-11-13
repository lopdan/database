describe 'database' do
    def run_script(commands)
      raw_output = nil
      IO.popen("./main.exe", "r+") do |pipe|
        commands.each do |command|
          pipe.puts command
        end
        pipe.close_write
        # Read entire output
        raw_output = pipe.gets(nil)
      end
      raw_output.split("\n")
    end
    it 'inserts and retrieves a row' do
      result = run_script([
        "insert 1 firstuser first@example.com",
        "select",
        ".exit",
      ])
      expect(result).to match_array([
        "Database > Executed.",
        "Database > (49, firstuser, first@example.com)",
        "Executed.",
        "Database > ",
      ])
    end
end