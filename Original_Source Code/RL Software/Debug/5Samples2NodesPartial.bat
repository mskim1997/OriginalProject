@echo off

echo Five Agents different goals sharing

for %%a in (1 2 3 4 5) do mpiexec -np 3 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 5 -o StatsFileFiveDiffAgentSharing%%a.csv -i AgentInfo1.txt -s true -h false

echo Five Agents different goals no sharing

for %%a in (1 2 3 4 5) do mpiexec -np 3 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 5 -o StatsFileFiveDiffAgent%%a.csv -i AgentInfo1.txt -s false -h false