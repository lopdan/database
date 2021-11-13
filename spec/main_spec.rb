describe 'Database' do
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
  it 'Insert and select test' do
    result = run_script([
      "insert 1 firstuser first@example.com",
      "select",
      ".exit",
    ])
    expect(result).to match_array([
      "Database > Executed.",
      "Database > (1, firstuser, first@example.com)",
      "Executed.",
      "Database > ",
    ])
  end
  it 'Error when table is full' do
    script = (1..1401).map do |i|
      "insert #{i} user#{i} user#{i}@example.com"
    end
    script << ".exit"
    result = run_script(script)
    expect(result[-2]).to eq('Database > Error: Table full.')
  end
  it 'Max lenght values insert' do
    long_username = "a"*32
    long_email = "a"*255
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
      "Database > Executed.",
      "Database > (1, #{long_username}, #{long_email})",
      "Executed.",
      "Database > ",
    ])
  end
  it 'Error when max length value is exceeded' do
    long_username = "a"*33
    long_email = "a"*256
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit",
    ]
    result = run_script(script)
    expect(result).to match_array([
      "Database > String is too long.",
      "Database > Executed.",
      "Database > ",
    ])
  end
end