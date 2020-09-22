% clc
% close all
% clear variables

% per simulare una chiamata dal main
%retCode = datPlotter("./benchMark/test/indexBenchMark.dat");

function out = datPlotter(argv)

% if (argc ~= 1)
%     print("Wrong format! use: @bash$ {PRGNAME} {PATHof.dat}\n");
% end

datValue = retrieveData(argv);

for i = 2+1:size(datValue, 2)
    fig = printGraph(datValue(:, [1 2 i]), i);
    
    k = strfind(argv, '/');
    
    folder = extractBefore(argv, k(end)); %% use last '/' to get path
    
    mkdir(folder + "/imgs");
    
    filename = datValue(:,i).Properties.VariableNames(1);
    
    path = folder + "/imgs/" + filename;
    
    saveas(fig, path, 'png');
end

out = 0;
end

function tab = retrieveData(path)
% to retrieve numbers data
num = readtable(path, 'VariableNamingRule','preserve');
tab = num;

end

function figObj = printGraph(subDat, index)

figObj = figure(index);

PlotName = subDat(:,1).Properties.VariableNames(1);
XName = subDat(:,2).Properties.VariableNames(1);
YName = subDat(:,3).Properties.VariableNames(1);


nCore = subDat.(1)(end); % max number of core



while(nCore > 0)
    
    c = subDat.(1);
    
    [indexes] = find(c == nCore);
    
    if (isempty(indexes)) % if we don't have considered this amount of core
        nCore = nCore - 1;
        continue;
    end
    
    valueTab = table2array( subDat(indexes(1):indexes(end),2:end) );
    
    subDat = subDat(1:indexes(1)-1, :); % we reduce size of subDat for the next iteration of find
    
    plot(valueTab(:,1), valueTab(:,2), '-*', ...
          'DisplayName', PlotName + ": " + nCore);
    grid on
    hold on
    
    if(indexes(1) == 1) % we reached the minimum amount of core
        break;
    end
    
    nCore = nCore - 1; 
end

% analyzed all data, adding some info and save on file

hold off


xlabel(XName, 'Interpreter', 'none');
ylabel(YName, 'Interpreter', 'none');

%legend('Interpreter','none', 'Location', 'best');

if(valueTab(1,2) < valueTab(end,2)) %% the graph is increasing
    position = 'Northwest';
else 
    position = 'Northeast';
end

legend('Interpreter','none', 'Location', position);

end
