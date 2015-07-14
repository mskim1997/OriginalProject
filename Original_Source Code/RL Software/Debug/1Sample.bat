@echo off
echo Turning Angle Run
echo angle = pi/6
for %%a in (1 2 3 4 5 6 7 8 9 10) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.167 -wi 5.0 -run %%a

echo angle = 5pi/12
for %%a in (11 12 13 14 15 16 17 18 19 20) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo angle = 7pi/12
for %%a in (21 22 23 24 25 26 27 28 29 30) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.58 -wi 5.0 -run %%a

echo angle = 3pi/4
for %%a in (31 32 33 34 35 36 37 38 39 40) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.75 -wi 5.0 -run %%a

echo angle = pi
for %%a in (41 42 43 44 45 46 47 48 49 50) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 1 -wi 5.0 -run %%a

echo Radius of Curvature Run
echo radius = 0.5
for %%a in (51 52 53 54 55 56 57 58 59 60) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 0.5 -im 4 -t 0.417 -wi 5.0 -run %%a

echo radius = 0.75
for %%a in (61 62 63 64 65 66 67 68 69 70) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 0.75 -im 4 -t 0.417 -wi 5.0 -run %%a

echo radius = 1.0
for %%a in (71 72 73 74 75 76 77 78 79 80) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo radius = 1.5
for %%a in (81 82 83 84 85 86 87 88 89 90) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.5 -im 4 -t 0.417 -wi 5.0 -run %%a

echo radius = 2.0
for %%a in (91 92 93 94 95 96 97 98 99 100) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 2.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Immediate Move Run
echo moves = 1
for %%a in (101 102 103 104 105 106 107 108 109 110) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 1 -t 0.417 -wi 5.0 -run %%a

echo moves = 2
for %%a in (111 112 113 114 115 116 117 118 119 120) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 2 -t 0.417 -wi 5.0 -run %%a

echo moves = 3
for %%a in (121 122 123 124 125 126 127 128 129 130) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 3 -t 0.417 -wi 5.0 -run %%a

echo moves = 4
for %%a in (131 132 133 134 135 136 137 138 139 140) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo moves = 5
for %%a in (141 142 143 144 145 146 147 148 149 150) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 5 -t 0.417 -wi 5.0 -run %%a

echo Look Ahead Run
echo look ahead = 5
for %%a in (151 152 153 154 155 156 157 158 159 160) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo look ahead = 6
for %%a in (161 162 163 164 165 166 167 168 169 170) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 6.0 -run %%a

echo look ahead = 7
for %%a in (171 172 173 174 175 176 177 178 179 180) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 7.0 -run %%a

echo look ahead = 8
for %%a in (181 182 183 184 185 186 187 188 189 190) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 8.0 -run %%a

echo look ahead = 10
for %%a in (191 192 193 194 195 196 197 198 199 200) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 10.0 -run %%a

echo agents, No sharing Run
echo agents = 1
for %%a in (201 202 203 204 205 206 207 208 209 210) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 2
for %%a in (211 212 213 214 215 216 217 218 219 220) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 2 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 3
for %%a in (221 222 223 224 225 226 227 228 229 230) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 3 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 4
for %%a in (231 232 233 234 235 236 237 238 239 240) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 4 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 5
for %%a in (241 242 243 244 245 246 247 248 249 250) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents, sharing Run
echo agents = 1
for %%a in (251 252 253 254 255 256 257 258 259 260) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 2
for %%a in (261 262 263 264 265 266 267 268 269 270) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 2 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 3
for %%a in (271 272 273 274 275 276 277 278 279 280) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 3 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 4
for %%a in (281 282 283 284 285 286 287 288 289 290) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 4 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo agents = 5
for %%a in (291 292 293 294 295 296 297 298 299 300) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Maze Run
echo Maze = 12x8
for %%a in (301 302 303 304 305 306 307 308 309 310) do mpiexec -np 2 MazeService.exe -m maze_12x8.in -x 8 -y 12 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Maze = 24x8
for %%a in (311 312 313 314 315 316 317 318 319 320) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Maze = 24x16
for %%a in (321 322 323 324 325 326 327 328 329 330) do mpiexec -np 2 MazeService.exe -m maze_24x16.in -x 16 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node Run
echo Node = 1
for %%a in (331 332 333 334 335 336 337 338 339 340) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node = 2
for %%a in (341 342 343 344 345 346 347 348 349 350) do mpiexec -np 3 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node = 4
for %%a in (361 362 363 364 365 366 367 368 369 370) do mpiexec -np 5 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 1 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s false -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Final Run
echo Node = 1, Agents = 5, Sharing
for %%a in (371 372 373 374 375 376 377 378 379 380) do mpiexec -np 2 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node = 2, Agents = 5, Sharing
for %%a in (381 382 383 384 385 386 387 388 389 390) do mpiexec -np 3 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a

echo Node = 4, Agents = 5, Sharing
for %%a in (401 402 403 404 405 406 407 408 409 410) do mpiexec -np 5 MazeService.exe -m maze_24x8.in -x 8 -y 24 -a 5 -o StatsFileOneAgent%%a.csv -i AgentInfo.txt -s true -h false -r 1.0 -im 4 -t 0.417 -wi 5.0 -run %%a