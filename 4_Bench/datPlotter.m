function out = datPlotter(path_badalpha, path_apache)

if (nargin ~= 2)
    disp("Error in datPlotter, naragin ~= 2")
    return;
end

%get list of all the files
filesTab_BA = readtable(path_badalpha, 'Delimiter', '\n');
disp(filesTab_BA)
filesTab_Ap = readtable(path_apache, 'Delimiter', '\n');
disp(filesTab_Ap)

%inizializate the faulty tests list
slash = strfind(path_badalpha, '/');
folder = extractBefore(path_badalpha, slash(end)); %% use last '/' to get path
fid = fopen(folder + "/faultyTests.txt", "w");
fprintf(fid, "Faulty experiments\n");

merged_dir = mkdir(folder + "/merged");

for i = 1:height(filesTab_BA) % we assume that we've made the same number of
    % tests per both the servers.
    file_BA = char(filesTab_BA.(1)(i));
    disp("Working on .dat file: " + file_BA);
    if (analizeDatFile(file_BA, "BA") == -1) % we save the file on a list of failed tests
        disp("Error in " + file_BA + " ; -1 found.")
        fprintf(fid, file_BA + '\n');
    end
    file_Ap= char(filesTab_Ap.(1)(i));
    disp("Working on .dat file: " + file_Ap);
    if (analizeDatFile(file_Ap, "Ap") == -1)
        disp("Error in " + file_Ap + " ; -1 found.")
        fprintf(fid, file_Ap + '\n');
    end
    % create the merged graph
    analyseMerged(file_BA, file_Ap, merged_dir);
end


fclose(fid);

fprintf('\n');
disp("######################")
disp("# Elaboration ended. #")
disp("######################")

out = 0;
end

function merg_file = analyseMerged(file_BA, file_Ap, merged_dir)

datValue_BA = retrieveData(file_BA);
datValue_Ap = retrieveData(file_Ap);

for i = 2+1: 5 % the first part of graphs
    disp("Start printing Graph n° " + i)
    fig_BA = printGraph(datValue_BA(:, [1 2 i]), i, file_BA);
    fig_Ap = printGraph(datValue_Ap(:, [1 2 i]), i, file_Ap);
    % copy elaboration in a new graph
    fig = copyobj(fig_BA,fig_Ap);
    
    disp("Printing Merged Graph " + i + " done")
    
    %save plot in a new image
    slash = strfind(file_BA, '/');
    dot = strfind(file_Ap, '.');
    folder = merged_dir + '/' + file_BA(slash(end):dot(end));
    w = warning('off','all');
    mkdir(folder);
    warning(w);
    
    filename = datValue_BA(:,i).Properties.VariableNames(1);
    
    path = folder + filename;
    disp("Saving in: " + path + ".png")
    saveas(fig, path, 'png');
end
fprintf('\n')

disp("Now saving the time response graph...")

nParCon = getParConn(datValue_BA);

for con = 1:length(nParCon)
    rows = datValue_BA.(2) == nParCon(con);
    resp_fig_Ba = printResponseTime(datValue_BA(rows, :), file_BA, "");
    resp_fig_Ap = printResponseTime(datValue_Ap(rows, :), file_Ap, "Ap");
    
    resp_fig = copyobj(resp_fig_Ba,resp_fig_Ap);
    
    
    path = folder + "TimeResp_" + nParCon(con);
    disp("Saving Response graph for " + nParCon(con) + ...
        " parallel connections in: ")
    fprintf('\t');
    disp(path + ".png")
    saveas(resp_fig, path, 'png');
end

disp("Analysis of .dat file done.")


merg_file = 0;
end


function sfile = analizeDatFile(file, type)

datValue = retrieveData(file);

if (any(datValue.(1) == -1))
    % we have at least an error in a sequence of tests
    sfile = -1;
    return;
end

for i = 2+1: 5 % the first part of graphs
    disp("Start printing Graph n° " + i)
    fig = printGraph(datValue(:, [1 2 i]), i, file);
    disp("Printing Graph " + i + " done")
    
    dot = strfind(file, '.');
    folder = extractBefore(file, dot(end)) + "/"; %% use last '.' to get path
    
    w = warning('off','all');
    mkdir(folder);
    warning(w);
    
    filename = datValue(:,i).Properties.VariableNames(1);
    
    path = folder + filename;
    disp("Saving in: " + path + ".png")
    saveas(fig, path, 'png');
end
fprintf('\n')

disp("Now saving the time response graph...")

nParCon = getParConn(datValue);

for con = 1:length(nParCon)
    rows = datValue.(2) == nParCon(con);
    resp_fig = printResponseTime(datValue(rows, :), file, type);
    
    path = folder + "TimeResp_" + nParCon(con);
    disp("Saving Response graph for " + nParCon(con) + ...
        " parallel connections in: ")
    fprintf('\t');
    disp(path + ".png")
    saveas(resp_fig, path, 'png');
end

disp("Analysis of .dat file done.")

sfile = 0;
end

function tab = retrieveData(path)
% to retrieve numbers data

tab = benchMarkTable(path);
end

function figObj = printGraph(subDat, index, fileName)

figObj = figure(index);

PlotName = subDat(:,1).Properties.VariableNames(1);
XName = subDat(:,2).Properties.VariableNames(1);
YName = subDat(:,3).Properties.VariableNames(1);
nMaxCore = subDat.(1)(end); % max number of core

if (nMaxCore == 1)
    displayName = "Apache";
else
    displayName = PlotName + ": " + nMaxCore;
end

while(nMaxCore > 0)
    
    c = subDat.(1);
    [indexes] = find(c == nMaxCore);
    
    if (isempty(indexes)) % if we don't have considered this amount of core
        nMaxCore = nMaxCore - 1;
        continue;
    end
    
    valueTab = table2array( subDat(indexes(1):indexes(end),2:end) );
    
    subDat = subDat(1:indexes(1)-1, :); % we reduce size of subDat for the next iteration of find
    
    plot(valueTab(:,1), valueTab(:,2), '-*', 'DisplayName', displayName);
    grid on
    hold on
    
    if(indexes(1) == 1) % we reached the minimum amount of core
        break;
    end
    
    nMaxCore = nMaxCore - 1;
end

hold off

% add some labels
xlabel(XName)
ylabel(YName)

slash = strfind(fileName, '/');
file = extractAfter(path, slash(end));
title(file)

legend('Interpreter','none', 'Location','best');

end

function parConn = getParConn(datValue)

indexConn = find(datValue.(1) == 1);

tmp = indexConn; % same array size

for ind = 1:length(indexConn)
    tmp(ind) = datValue.(2)(ind);
end

parConn = tmp;

end

function resp_fig = printResponseTime(reducedDat, fileName, type)
%reducedDat is only shorter, not slimmer

resp_fig = figure(22);

numConn = reducedDat.(2)(1);

% we have to discard some columns for plotting
valueTab = table2array(reducedDat(:, [1 6:end]));

percentage = [1 50 66 75 80 90 95 98 99 100]; % WARNING, w/o min in this case

for row = 1: size(valueTab,1)
    
    for i = 1:length(valueTab(row,2:end))
        if(valueTab(row,i) == 0)
            valueTab(row,i) = 1; % Il diagramma semilogaritmico non vuole 0
        end
    end
    
    if (type == "Ap")
        displayName = "Apache";
    else
        displayName = "N°Worker: " + valueTab(row,1);
    end
    
    % using 2 below, we include minimum response time! [default = 3]
    %     semilogy(percentage, valueTab(row,2:end), '-*', ...
    %         'DisplayName', "N°Worker: " + valueTab(row,1));
    %     semilogx(valueTab(row,2:end),percentage, '-*', ...
    %         'DisplayName', "N°Worker: " + valueTab(row,1));
    plot(valueTab(row,2:end),percentage, '-*', ...
        'DisplayName', displayName);
    hold on
    grid on
end

hold off

% TODO: Nome del file di test
slash = strfind(fileName, '/');
file = extractAfter(path, slash(end));

title("'" + file + "': " + "Time Response with " + numConn + " parallel connections")
% ylabel("Time [ms]")
% xlabel("Percentage [%]")
xlabel("Time [ms]")
ylabel("Percentage [%]")

legend('Interpreter','none', 'Location','best');

end