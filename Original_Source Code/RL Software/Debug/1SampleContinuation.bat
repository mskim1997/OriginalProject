echo Node = 4
for %%a in (365 366 367 368 369 370) do mpiexec -np 5 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Final Run
echo Node = 1, Agents = 5, Sharing
for %%a in (371 372 373 374 375 376 377 378 379 380) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node = 2, Agents = 5, Sharing
for %%a in (381 382 383 384 385 386 387 388 389 390) do mpiexec -np 3 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node = 4, Agents = 5, Sharing
for %%a in (401 402 403 404 405 406 407 408 409 410) do mpiexec -np 5 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a