@echo off
echo One Agent
for %%a in (1 2 3 4 5) do mpiexec -np 2 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false

echo Five Agents Sharing

for %%a in (1 2 3 4 5) do mpiexec -np 2 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 5 -o StatsFileFiveAgentSharing%%a.csv -i AgentInfo.txt -s true -h false

echo Five Agents No sharing

for %%a in (1 2 3 4 5) do mpiexec -np 2 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 5 -o StatsFileFiveAgent%%a.csv -i AgentInfo.txt -s false -h false

echo Five Agents different goals sharing

for %%a in (1 2 3 4 5) do mpiexec -np 2 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 5 -o StatsFileFiveDiffAgentSharing%%a.csv -i AgentInfo1.txt -s true -h false

echo Five Agents different goals no sharing

for %%a in (1 2 3 4 5) do mpiexec -np 2 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 5 -o StatsFileFiveDiffAgent%%a.csv -i AgentInfo1.txt -s false -h false