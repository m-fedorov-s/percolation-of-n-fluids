#!/usr/bin/env python
# coding: utf-8


import numpy as np
import pandas as pd
from pathlib import Path
from matplotlib import pyplot as plt
import seaborn as sns
sns.set_style("whitegrid")
# sns.set_context("paper") # "paper" or "talk"
from tqdm.auto import tqdm
from scipy.stats import norm


def get_bernoulli_confidence_interval(estimates, samples, confidence=0.95):
    quantile = norm.ppf(confidence)
    mid = (estimates + quantile ** 2 / samples / 2) / (1 + quantile ** 2 / samples)
    elbow = np.sqrt(estimates * (1 - estimates) / samples + quantile ** 2 / (4 * samples ** 2)) * quantile / (1 + quantile ** 2 / samples)
    return np.vstack([np.maximum(mid - elbow, 0), np.minimum(mid + elbow, 1)])


def load_data(path_str):
    data_path = Path(path_str)
    data = []
    for file in tqdm(data_path.iterdir(), desc="Reading data", total=len(list(data_path.iterdir()))):
        metadata = file.stem.split('_')
        liquids = int(metadata[metadata.index('liquids') + 1])
        size = int(metadata[metadata.index('size') + 1])
        raw_data = pd.read_csv(file, header=None, names=[f"{i}" for i in range(liquids + 1)])
        raw_data.drop(f"{liquids}", axis=1, inplace=True)
        frame = pd.DataFrame(columns=["total_liquids", "size", "percolated_liquids", "first_percolated"])
        frame["first_percolated"] = raw_data["0"]
        frame["percolated_liquids"] = raw_data.sum(axis=1)
        frame["size"] = size
        frame["total_liquids"] = liquids
        data.append(frame)

    data = pd.concat(data)
    return data


def plot_percolation_fraction_depending_on_size(data, language="eng"):
    for liquid in tqdm(sorted(data.total_liquids.unique()),
                       leave=False,
                       desc="Generating percolation plots"):
        sns.set_context("talk")
        if data[data.total_liquids == liquid]['size'].nunique() < 2:
            break
        if liquid == 3:
            plt.figure(figsize=(8, 6))
        else:
            plt.figure(figsize=(16, 6))
        sns.lineplot(data=data[data.total_liquids == liquid].groupby('size')["percolated_liquids"].value_counts(normalize=True).unstack().fillna(0),
                    palette=('Greys'))
        if language == "rus":
            plt.ylabel('Доля исходов с протеканием\nданного количества жидкостей')
            plt.xlabel('Размер шестиугольника $s$')
            plt.legend(title="Количество протекших жидкостей", title_fontsize="small")
        elif language == "eng":
            plt.ylabel('Fraction of samples with given\nnumber of percolated liquids')
            plt.xlabel('Size $s$ of the hexagon')
            plt.legend(title="The number of percolated liquids", title_fontsize="small")
        else:
            raise ValueError(f"Unknown language {language}")
        plt.savefig(f'./output_images/{language}__percolation__liquids={liquid}.pdf', format="pdf")
        plt.close()

def plot_four_cdf_grid(data, dims=[5, 25, 50, 100], size=100, language="eng"):
    sns.set_context("paper")
    fig, axs = plt.subplots(2, 2, figsize=(6, 4))
    fig.tight_layout(pad=2)
    for ax, liquids in zip([ax for axline in axs for ax in axline], dims):
    #     print(ax, liquids)
        p_s = data[(data['size'] == size) & (data["total_liquids"] == liquids)].first_percolated.mean()
        estimates = data[(data['size'] == size) & (data["total_liquids"] == liquids)]["percolated_liquids"] / (np.sqrt(liquids)) - np.sqrt(liquids) * p_s
        estimates /= np.sqrt((1 - p_s) * p_s)
        steps = estimates.value_counts(normalize=True).sort_index()
        if language == "rus":
            cdf_label = 'Нормированная функция\nраспределения $\overline{X_{n, s}}$'
            std_label = 'Стандартное нормальное\nраспределение'
        elif language == "eng":
            cdf_label = 'Normalized CDF of $\overline{X_{n, s}}$'
            std_label = 'Standard Gaussian CDF'
        else:
            raise ValueError(f"Unknown language {language}")
        ax.stairs(np.cumsum(np.concatenate([[0], steps.values])),
                np.concatenate([[steps.index.min() - 1], steps.index, [steps.index.max() + 1]]),
                baseline=None,
                color='black',
                label=cdf_label)
    #     print(steps.values)
        x=np.linspace(steps.index.min() - 1, steps.index.max() + 1, num=1000)
        ax.plot(x, norm.cdf(x), label=std_label, color='black', linestyle='dashed')
        ax.set_title(f"$n$={liquids}", fontdict={'fontsize':15})
        ax.legend(fontsize='x-small', loc='upper left')
        
    plt.savefig(f"./output_images/{language}__four_grid_cdf_for_size={size}_n={dims[0]},{dims[1]},{dims[2]},{dims[3]}.pdf", format="pdf")


def plot_percolation_fraction_with_confidence_intervals(data, liquids=3, language="eng"):
    liquids = 3
    part = pd.get_dummies(data[data.total_liquids == liquids].percolated_liquids)
    part['size'] = data[data.total_liquids == liquids]['size']
    sns.lineplot(data=part.set_index('size'), palette=('Greys'))

    if language == "rus":
        plt.ylabel('Доля исходов с протеканием\nданного количества жидкостей')
        plt.xlabel('Размер шестиугольника $s$')
        plt.legend(title="Количество протекших жидкостей", title_fontsize="small")
    elif language == "eng":
        plt.ylabel('Fraction of samples with given\nnumber of percolated liquids')
        plt.xlabel('Size $s$ of the hexagon')
        plt.legend(title="The number of percolated liquids", title_fontsize="small")
    else:
        raise ValueError(f"Unknown language {language}")
    plt.savefig(f'./output_images/{language}__percolation_with_confidence__liquids={liquids}.pdf', format="pdf")


def plot_ratio_all_to_one(data, confidence_level=0.95, language="eng"):
    sns.set_context("talk")
    for liquid in tqdm(sorted(data.total_liquids.unique()),
                       leave=False,
                       desc="Generating plots with confidence intervals"):
        part = data[(data.total_liquids == liquid) & (data['size'] % 5 == 0)]
        if liquid not in part["percolated_liquids"].unique():
            continue
        probs_all = part.groupby("size")["percolated_liquids"].value_counts(normalize=True).unstack().fillna(0)[liquid]
        probs_one = part.groupby("size")["first_percolated"].mean()

        samples = part.groupby("size")["percolated_liquids"].count()
        bounds_all = get_bernoulli_confidence_interval(probs_all, samples, confidence=np.sqrt(confidence_level))
        bounds_one = get_bernoulli_confidence_interval(probs_one, samples, confidence=np.sqrt(confidence_level))
        values = probs_all / (probs_one ** liquid)
        bounds = bounds_all / (bounds_one[::-1, :] ** liquid)

        errors = np.abs(bounds - values.values)
        fig = plt.figure(figsize=(16,6))
        plt.errorbar(probs_all.index, values, capsize=2, yerr=errors, label='', color='black', elinewidth=0.5)
        if language == "rus":
            plt.ylabel('Отношене вероятностей протекания всех жидкостей и\n $n$-ой степени вероятности протекания первой жидкости')
            plt.xlabel('Размер $s$ шестиугольника')
        elif language == "eng":
            plt.ylabel('Ratio of the probability that all the liquids percolated to\nthe $n$-th power of the probability that the first liquid percolated')
            plt.xlabel('Size $s$ of hexagon')
        else:
            raise ValueError(f"Unknown language {language}")
        plt.savefig(f'./output_images/{language}__ratio_all_to_one__liquids={liquid}_confidence={confidence_level}.pdf', format="pdf")
        plt.close(fig)


if __name__ == "__main__":
    languages = ["eng", "rus"]
    language = input("Choose language. [all]/eng/rus: ").lower().strip()
    if language in languages:
        languages = list(language)
    else:
        language = "all"
    print(f"Chosen option: {language}")

    data = load_data("./results_csv/")
    for language in languages:
        print(f"Generating plots for language: {language}")
        plot_percolation_fraction_depending_on_size(data, language=language)
        plot_four_cdf_grid(data, language=language)
        plot_ratio_all_to_one(data, language=language)
    print("Done! All images saved in the folder ./output_images/")
